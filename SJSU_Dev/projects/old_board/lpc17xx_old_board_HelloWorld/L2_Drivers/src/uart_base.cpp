/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */


#include "UART_Base.hpp"
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "task.h"           // taskYield();
#include "isr_priorities.h" // Uart priority
#include "rit_isr.h"        // rit_get_timer_value_ms();

bool UART_Base::getChar(char* pInputChar, unsigned int timeout) const
{
    if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState()) {
        return xQueueReceive(mRxQueue, pInputChar, timeout);
    }
    else {
        long dummy = 0;
        unsigned int timeout_of_char = rit_get_timer_value_ms() + timeout;
        while (! xQueueReceiveFromISR(mRxQueue, pInputChar, &dummy)) {
            if (rit_get_timer_value_ms() > timeout_of_char) {
                return false;
            }
        }
    }
    return true;
}

bool UART_Base::putChar(char out, unsigned int timeout)
{
    /* If OS not running, just send data using polling and return */
    if (taskSCHEDULER_RUNNING != xTaskGetSchedulerState()) {
        mpUARTRegBase->THR = out;
        while(! (mpUARTRegBase->LSR & (1 << 6)));
        return true;
    }

    /* FreeRTOS running, so send to queue and if queue is full, return false */
    long yieldRequired = 0;
    if(! xQueueSend(mTxQueue, &out, timeout)) {
        return false;
    }

    vPortEnterCritical();
    {
        /**
         * We queued our char above, but if we are not expecting THRE interrupt,
         * then dequeue this char and send it out.
         */
        if (!mExpectingThreInterrupt || (mpUARTRegBase->LSR & (1 << 6)) )
        {
            xQueueReceiveFromISR(mTxQueue, &out, &yieldRequired);
            mpUARTRegBase->THR = out;
            mExpectingThreInterrupt = true;
        }
        else {
            /**
             * We are expecting an interrupt to handle this case
             */
        }
    }
    vPortExitCritical();

    return true;
}

bool UART_Base::recentlyActive(unsigned int ms) const
{
    portTickType lastTimeStampMs = MS_PER_TICK() * mLastActivityTime;
    portTickType currentTimeMs = xTaskGetMsCount();
    return (currentTimeMs - lastTimeStampMs) < ms;
}

void UART_Base::resetActivity(void)
{
    mLastActivityTime = xTaskGetMsCount();
}

void UART_Base::setBaudRate(unsigned int baudRate)
{
    mpUARTRegBase->LCR = (1 << 7); // Enable DLAB to access DLM, DLL, and IER
    {
        uint16_t bd = (mPeripheralClock / (16 * baudRate)) + 0.5;
        mpUARTRegBase->DLM = (bd >> 8);
        mpUARTRegBase->DLL = (bd >> 0);
    }
    mpUARTRegBase->LCR = 3; // Disable DLAB and set 8bit per char
}

void UART_Base::handleInterrupt()
{
    /**
     * Bit Masks of IIR register Bits 3:1 that contain interrupt reason.
     * Bits are shifted left because reasonForInterrupt contains Bits 3:0
     */
    const unsigned int transmitterEmpty = (1 << 1);
    const unsigned int dataAvailable    = (2 << 1);
    const unsigned int dataTimeout      = (6 << 1);

    long higherPriorityTaskWoken = 0;
    long switchRequired = 0;
    char c = 0;
    unsigned charsSent = 0;

    uint16_t reasonForInterrupt = (mpUARTRegBase->IIR & 0xE);
    {
        /**
         * If multiple sources of interrupt arise, let this interrupt exit, and re-enter
         * for the new source of interrupt.
         */
        switch (reasonForInterrupt)
        {
            case transmitterEmpty:
            {
                if(uxQueueMessagesWaitingFromISR(mTxQueue) > mTxQWatermark) {
                    mTxQWatermark = uxQueueMessagesWaitingFromISR(mTxQueue);
                }

                /**
                 * When THRE (Transmit Holding Register Empty) interrupt occurs,
                 * we can send as many bytes as the hardware FIFO supports (16)
                 */
                const unsigned char hwTxFifoSize = 16;
                for(charsSent=0;
                        charsSent < hwTxFifoSize && xQueueReceiveFromISR(mTxQueue, &c, &higherPriorityTaskWoken);
                        charsSent++)
                {
                    mpUARTRegBase->THR = c;
                    if(higherPriorityTaskWoken) {
                        switchRequired = 1;
                    }
                }
                mExpectingThreInterrupt = (charsSent > 0);
            }
            break;

            case dataAvailable:
            case dataTimeout:
            {
                mLastActivityTime = xTaskGetTickCountFromISR();
                /**
                 * While receive Hardware FIFO not empty, keep queuing the data.
                 * Even if xQueueSendFromISR() Fails (Queue is full), we still need to
                 * read RBR register otherwise interrupt will not clear
                 */
                while (0 != (mpUARTRegBase->LSR & (1 << 0)))
                {
                    c = mpUARTRegBase->RBR;
                    xQueueSendFromISR(mRxQueue, &c, &higherPriorityTaskWoken);
                    if(higherPriorityTaskWoken) {
                        switchRequired = 1;
                    }
                }

                if(uxQueueMessagesWaitingFromISR(mRxQueue) > mRxQWatermark) {
                    mRxQWatermark = uxQueueMessagesWaitingFromISR(mRxQueue);
                }
            }
            break;

            default:
                /* Read LSR register to clear Line Status Interrupt */
                reasonForInterrupt = mpUARTRegBase->LSR;
                break;
        }
    }

    portEND_SWITCHING_ISR(switchRequired);
}

///////////////
// Protected //
///////////////
UART_Base::UART_Base(unsigned int* pUARTBaseAddr) :
        mpUARTRegBase((LPC_UART_TypeDef*) pUARTBaseAddr),
        mExpectingThreInterrupt(false),
        mPeripheralClock(0),
        mRxQWatermark(0),
        mTxQWatermark(0)
{
}

bool UART_Base::init(unsigned int pclk, unsigned int baudRate,
                        int rxQSize, int txQSize)
{
    mPeripheralClock = pclk;

    // Configure UART Hardware: Baud rate, FIFOs etc.
    if (LPC_UART0_BASE == (unsigned int) mpUARTRegBase)
    {
        LPC_SC->PCONP |= (1 << 3); // Enable UART0
        NVIC_EnableIRQ(UART0_IRQn);
    }
    /*
     else if(LPC_UART1_BASE == (unsigned int)mpUARTRegBase)
     {
     LPC_SC->PCONP |= (1 << 4); // Enable UART1
     NVIC_EnableIRQ(UART1_IRQn);
     }
     */
    else if (LPC_UART2_BASE == (unsigned int) mpUARTRegBase)
    {
        LPC_SC->PCONP |= (1 << 24); // Enable UART2
        NVIC_EnableIRQ(UART2_IRQn);
    }
    else if (LPC_UART3_BASE == (unsigned int) mpUARTRegBase)
    {
        LPC_SC->PCONP |= (1 << 25); // Enable UART3
        NVIC_EnableIRQ(UART3_IRQn);
    }
    else
    {
        return false;
    }

    // Enable & Reset FIFOs and set 4 char timeout for Rx
    mpUARTRegBase->FCR = (1 << 0) | (1 << 6);
    mpUARTRegBase->FCR |= (1 << 1) | (1 << 2);

    setBaudRate(baudRate);

    // Set minimum queue size
    if (rxQSize < 9) rxQSize = 8;
    if (txQSize < 9) txQSize = 8;

    // Create the receive and transmit queues
    mRxQueue = xQueueCreate(rxQSize, sizeof(char));
    mTxQueue = xQueueCreate(txQSize, sizeof(char));

    // Enable Rx/Tx and line status Interrupts:
    mpUARTRegBase->IER = (1 << 0) | (1 << 1) | (1 << 2); // B0:Rx, B1: Tx

    return (0 != mRxQueue && 0 != mTxQueue);
}
