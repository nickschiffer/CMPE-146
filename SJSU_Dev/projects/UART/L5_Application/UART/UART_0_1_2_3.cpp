/*
 * UART_0_1_2_3.cpp
 *
 *  Created on: Mar 9, 2019
 *      Author: nickschiffer (nick@schiffer.us)
 */

#include <UART/UART_0_1_2_3.hpp>

QueueHandle_t LabUart::u0_rx_queue;
QueueHandle_t LabUart::u1_rx_queue;
QueueHandle_t LabUart::u2_rx_queue;
QueueHandle_t LabUart::u3_rx_queue;

LabUart::~LabUart()
{
    //TODO disable and cleanup device
}

int LabUart::Initialize(UART_Device device, BAUD_Rate baud_rate,
        bool rx_interrupt_enable, Frame_Size frame_size, Stop_Bit stop_bit, bool break_control,
        bool parity_enable, Parity_Mode parity_mode)
{

    switch (device){
        case U0:
        case U1:
        case U2:
        case U3:{
            /*
             * Power on UART Device
             */
            switch(device){
                case U0:
                    LPC_SC->PCONP |= (1 << pconp_uart0);
                    break;
                case U1:
                    LPC_SC->PCONP |= (1 << pconp_uart1);
                    break;
                case U2:
                    LPC_SC->PCONP |= (1 << pconp_uart2);
                    break;
                case U3:
                    LPC_SC->PCONP |= (1 << pconp_uart3);
                    break;
            }

            /*
             * Select Peripheral Clock Divider
             * Select 01 for CCLK/1
             */
                switch(device){
                    case U0:
                        LPC_SC->PCLKSEL0 &= ~(1 << 7);
                        LPC_SC->PCLKSEL0 |=  (1 << 6);
                        break;
                    case U1:
                        LPC_SC->PCLKSEL0 &= ~(1 << 9);
                        LPC_SC->PCLKSEL0 |=  (1 << 8);
                        break;
                    case U2:
                        LPC_SC->PCLKSEL1 &= ~(1 << 17);
                        LPC_SC->PCLKSEL1 |=  (1 << 16);
                        break;
                    case U3:
                        LPC_SC->PCLKSEL1 &= ~(1 << 19);
                        LPC_SC->PCLKSEL1 |=  (1 << 18);
                        break;
                }

            /*
             * Select TX and RX pins
             */
                switch(device){
                    case U0:
                        LPC_PINCON->PINSEL0 |=  (1 << 4);
                        LPC_PINCON->PINSEL0 &= ~(1 << 5);
                        LPC_PINCON->PINSEL0 |=  (1 << 6);
                        LPC_PINCON->PINSEL0 &= ~(1 << 7);
                        break;
                    case U1:
                        LPC_PINCON->PINSEL4 |=  (1 << 1);
                        LPC_PINCON->PINSEL4 &= ~(1 << 0);
                        LPC_PINCON->PINSEL4 |=  (1 << 3);
                        LPC_PINCON->PINSEL4 &= ~(1 << 2);
                        break;
                    case U2:
                        LPC_PINCON->PINSEL4 |=  (1 << 17);
                        LPC_PINCON->PINSEL4 &= ~(1 << 16);
                        LPC_PINCON->PINSEL4 |=  (1 << 19);
                        LPC_PINCON->PINSEL4 &= ~(1 << 18);
                        break;
                    case U3:
                        LPC_PINCON->PINSEL9 |=  (0b11 << 24);
                        LPC_PINCON->PINSEL9 |=  (0b11 << 26);
                        break;
                }
            /*
             * Enable pullup on TX pin (not recommended on RX pin)
             */
                switch(device){
                    case U0:
                        LPC_PINCON->PINMODE0 &= ~(0b11 << 4);
                        break;
                    case U1:
                        LPC_PINCON->PINMODE4 &= ~(0b11 << 0);
                        break;
                    case U2:
                        LPC_PINCON->PINMODE4 &= ~(0b11 << 16);
                        break;
                    case U3:
                        LPC_PINCON->PINMODE9 &= ~(0b11 << 24);
                        break;
                }
            /*
             * Select requested frame size
             */
                switch (frame_size){
                    case f5_bit:
                        /*
                         * Set frame size to 5 (00)
                         */
                        switch(device){
                            case U0:
                                LPC_UART0->LCR &= ~(0b11 << 0);
                                break;
                            case U1:
                                LPC_UART1->LCR &= ~(0b11 << 0);
                                break;
                            case U2:
                                LPC_UART2->LCR &= ~(0b11 << 0);
                                break;
                            case U3:
                                LPC_UART3->LCR &= ~(0b11 << 0);
                                break;
                        }
                        break;
                    case f6_bit:
                        /*
                         * Set frame size to 6 (01)
                         */
                        switch(device){
                            case U0:
                                LPC_UART0->LCR &= ~(1 << 1);
                                LPC_UART0->LCR |=  (1 << 0);
                                break;
                            case U1:
                                LPC_UART1->LCR &= ~(1 << 1);
                                LPC_UART1->LCR |=  (1 << 0);
                                break;
                            case U2:
                                LPC_UART2->LCR &= ~(1 << 1);
                                LPC_UART2->LCR |=  (1 << 0);
                                break;
                            case U3:
                                LPC_UART3->LCR &= ~(1 << 1);
                                LPC_UART3->LCR |=  (1 << 0);
                                break;
                        }
                        break;
                    case f7_bit:
                        /*
                         * Set frame size to 7 (10)
                         */
                        switch(device){
                            case U0:
                                LPC_UART0->LCR &= ~(1 << 0);
                                LPC_UART0->LCR |=  (1 << 1);
                                break;
                            case U1:
                                LPC_UART1->LCR &= ~(1 << 0);
                                LPC_UART1->LCR |=  (1 << 1);
                                break;
                            case U2:
                                LPC_UART2->LCR &= ~(1 << 0);
                                LPC_UART2->LCR |=  (1 << 1);
                                break;
                            case U3:
                                LPC_UART3->LCR &= ~(1 << 0);
                                LPC_UART3->LCR |=  (1 << 1);
                                break;
                        }
                        break;
                    case f8_bit:
                        /*
                         * Set frame size to 7 (11)
                         */
                        switch(device){
                            case U0:
                                LPC_UART0->LCR |= (0b11 << 0);
                                break;
                            case U1:
                                LPC_UART1->LCR |= (0b11 << 0);
                                break;
                            case U2:
                                LPC_UART2->LCR |= (0b11 << 0);
                                break;
                            case U3:
                                LPC_UART3->LCR |= (0b11 << 0);
                                break;
                        }
                        break;
                    default:
                        return Unspecified_Error;
                }
            /*
             * Select requested number of stop bits
             */
                switch (stop_bit){
                    case s1_bit:
                        /*
                         * 1 stop bit
                         */
                        switch(device){
                            case U0:
                                LPC_UART0->LCR &= ~(1 << 2);
                                break;
                            case U1:
                                LPC_UART1->LCR &= ~(1 << 2);
                                break;
                            case U2:
                                LPC_UART2->LCR &= ~(1 << 2);
                                break;
                            case U3:
                                LPC_UART3->LCR &= ~(1 << 2);
                                break;
                        }
                        break;
                    case s2_bit:
                        /*
                         * 2 stop bits
                         */
                        switch(device){
                            case U0:
                                LPC_UART0->LCR |=  (1 << 2);
                                break;
                            case U1:
                                LPC_UART1->LCR |=  (1 << 2);
                                break;
                            case U2:
                                LPC_UART2->LCR |=  (1 << 2);
                                break;
                            case U3:
                                LPC_UART3->LCR |=  (1 << 2);
                                break;
                        }
                        break;
                    default:
                        return Unspecified_Error;
                }
            /*
             * Enable Parity and parity mode if requested
             */
                if (parity_enable){
                    switch(device){
                        case U0:
                            LPC_UART0->LCR |= (1 << 3);
                            break;
                        case U1:
                            LPC_UART1->LCR |= (1 << 3);
                            break;
                        case U2:
                            LPC_UART2->LCR |= (1 << 3);
                            break;
                        case U3:
                            LPC_UART3->LCR |= (1 << 3);
                            break;
                    }
                    switch (parity_mode){
                        case pOdd:
                            /*
                             * Odd parity. Number of 1s in the transmitted
                             * character and the attached parity bit will be odd.
                             */
                            switch(device){
                                case U0:
                                    LPC_UART0->LCR &= ~(0b11 << 4);
                                    break;
                                case U1:
                                    LPC_UART1->LCR &= ~(0b11 << 4);
                                    break;
                                case U2:
                                    LPC_UART2->LCR &= ~(0b11 << 4);
                                    break;
                                case U3:
                                    LPC_UART3->LCR &= ~(0b11 << 4);
                                    break;
                            }
                            break;
                        case pEven:
                            /*
                             * Even Parity. Number of 1s in the transmitted
                             * character and the attached parity bit will be even.
                             */
                            switch(device){
                                case U0:
                                    LPC_UART0->LCR &= ~(0b11 << 4);
                                    LPC_UART0->LCR |=  (1 << 4);
                                    break;
                                case U1:
                                    LPC_UART1->LCR &= ~(0b11 << 4);
                                    LPC_UART1->LCR |=  (1 << 4);
                                    break;
                                case U2:
                                    LPC_UART2->LCR &= ~(0b11 << 4);
                                    LPC_UART2->LCR |=  (1 << 4);
                                    break;
                                case U3:
                                    LPC_UART3->LCR &= ~(0b11 << 4);
                                    LPC_UART3->LCR |=  (1 << 4);
                                    break;
                            }
                            break;
                        case pForced_1:
                            /*
                             * Forced "1" stick parity.
                             */
                            switch(device){
                                case U0:
                                    LPC_UART0->LCR &= ~(0b11 << 4);
                                    LPC_UART0->LCR |=  (1 << 5);
                                    break;
                                case U1:
                                    LPC_UART1->LCR &= ~(0b11 << 4);
                                    LPC_UART1->LCR |=  (1 << 5);
                                    break;
                                case U2:
                                    LPC_UART2->LCR &= ~(0b11 << 4);
                                    LPC_UART2->LCR |=  (1 << 5);
                                    break;
                                case U3:
                                    LPC_UART3->LCR &= ~(0b11 << 4);
                                    LPC_UART3->LCR |=  (1 << 5);
                                    break;
                            }
                            break;
                        case pForced_2:
                            /*
                             * Forced "0" stick parity.
                             */
                            switch(device){
                                case U0:
                                    LPC_UART0->LCR |= (0b11 << 4);
                                    break;
                                case U1:
                                    LPC_UART1->LCR |= (0b11 << 4);
                                    break;
                                case U2:
                                    LPC_UART2->LCR |= (0b11 << 4);
                                    break;
                                case U3:
                                    LPC_UART3->LCR |= (0b11 << 4);
                                    break;
                            }
                            break;
                        default:
                            return Invalid_Parity_Config;
                    }
                }
                else {
                    switch(device){
                        case U0:
                            LPC_UART0->LCR &= ~(1 << 3);
                            break;
                        case U1:
                            LPC_UART1->LCR &= ~(1 << 3);
                            break;
                        case U2:
                            LPC_UART2->LCR &= ~(1 << 3);
                            break;
                        case U3:
                            LPC_UART3->LCR &= ~(1 << 3);
                            break;
                    }
                    if (parity_mode != pNone)
                        return Invalid_Parity_Config;
                }
            /*
             * Enable Break Control if Requested
             */
                switch(device){
                    case U0:
                        break_control ? (LPC_UART0->LCR |= (1 << 6)) : (LPC_UART0->LCR &= ~(1 << 6));
                        break;
                    case U1:
                        break_control ? (LPC_UART1->LCR |= (1 << 6)) : (LPC_UART1->LCR &= ~(1 << 6));
                        break;
                    case U2:
                        break_control ? (LPC_UART2->LCR |= (1 << 6)) : (LPC_UART2->LCR &= ~(1 << 6));
                        break;
                    case U3:
                        break_control ? (LPC_UART3->LCR |= (1 << 6)) : (LPC_UART3->LCR &= ~(1 << 6));
                        break;
                }
            /*
             * Set requested baud rate (see baud rate formula)
             */
                uint32_t pclk;
                uint8_t clk_div;
                switch(device){
                    case U0:
                        clk_div = ((LPC_SC->PCLKSEL1 >> 6) & 0x3);
                        break;
                    case U1:
                        clk_div = ((LPC_SC->PCLKSEL0 >> 8) & 0x3);
                        break;
                    case U2:
                        clk_div = ((LPC_SC->PCLKSEL1 >> 16) & 0x3);
                        break;
                    case U3:
                        clk_div = ((LPC_SC->PCLKSEL1 >> 18) & 0x3);
                        break;
                }
                switch (clk_div){
                    case clkdiv_1:
                        pclk = sys_get_cpu_clock();
                        break;
                    case clkdiv_2:
                        pclk = sys_get_cpu_clock() >> 1;
                        break;
                    case clkdiv_4:
                        pclk = sys_get_cpu_clock() >> 2;
                        break;
                    case clkdiv_8:
                        pclk = sys_get_cpu_clock() >> 3;
                        break;
                    default:
                        return Unspecified_Error;
                }
                uint16_t dl = (uint16_t)(pclk / ((uint32_t)baud_rate << 4));
                    /*
                     * Set DLAB bit to 1 to access DLL and DLM registers
                     */
                        switch (device){
                            case U0:
                                LPC_UART0->LCR |= (1 << 7);
                                break;
                            case U1:
                                LPC_UART1->LCR |= (1 << 7);
                                break;
                            case U2:
                                LPC_UART2->LCR |= (1 << 7);
                                break;
                            case U3:
                                LPC_UART3->LCR |= (1 << 7);
                                break;
                            default:
                                return Unspecified_Error;
                        }
                    /*
                     * Set DLM and DLL registers
                     */
                        switch (device){
                            case U0:
                                LPC_UART0->DLL = (dl & 0xFF);
                                LPC_UART0->DLM = ((dl >> 8) & 0xFF);
                                break;
                            case U1:
                                LPC_UART1->DLL = (dl & 0xFF);
                                LPC_UART1->DLM = ((dl >> 8) & 0xFF);
                                break;
                            case U2:
                                LPC_UART2->DLL = (dl & 0xFF);
                                LPC_UART2->DLM = ((dl >> 8) & 0xFF);
                                break;
                            case U3:
                                LPC_UART3->DLL = (dl & 0xFF);
                                LPC_UART3->DLM = ((dl >> 8) & 0xFF);
                                break;
                            default:
                                return Unspecified_Error;
                        }
            /*
             * Enable RX Interrupt if requested
             */
                /*
                 * Enable RX Interrupt (need to set DLAB to 0 first)
                 */
                    switch (device){
                        case U0:
                            LPC_UART0->LCR &= ~(1 << 7);
                            if (rx_interrupt_enable){
                                LPC_UART0->IER |=  (1 << 0);
                                NVIC_EnableIRQ(UART0_IRQn);
                                isr_register(UART0_IRQn, uart0_rx_intr);
                                u0_rx_queue = xQueueCreate(QUEUE_SIZE, sizeof(char));
                                intr_enabled = true;

                            }
                            else
                                LPC_UART0->IER &= ~(1 << 0);
                            break;
                        case U1:
                            LPC_UART1->LCR &= ~(1 << 7);
                            if (rx_interrupt_enable){
                                LPC_UART1->IER |=  (1 << 0);
                                NVIC_EnableIRQ(UART1_IRQn);
                                isr_register(UART1_IRQn, uart1_rx_intr);
                                u1_rx_queue = xQueueCreate(QUEUE_SIZE, sizeof(char));
                                intr_enabled = true;
                            }
                            else
                                LPC_UART1->IER &= ~(1 << 0);
                            break;
                        case U2:
                            LPC_UART2->LCR &= ~(1 << 7);
                            if (rx_interrupt_enable){
                                LPC_UART2->IER |=  (1 << 0);
                                NVIC_EnableIRQ(UART2_IRQn);
                                isr_register(UART2_IRQn, uart2_rx_intr);
                                u2_rx_queue = xQueueCreate(QUEUE_SIZE, sizeof(char));
                                intr_enabled = true;
                            }
                            else
                                LPC_UART2->IER &= ~(1 << 0);
                            break;
                        case U3:
                            LPC_UART3->LCR &= ~(1 << 7);
                            if (rx_interrupt_enable){
                                LPC_UART3->IER |=  (1 << 0);
                                NVIC_EnableIRQ(UART3_IRQn);
                                isr_register(UART3_IRQn, uart3_rx_intr);
                                u3_rx_queue = xQueueCreate(QUEUE_SIZE, sizeof(char));
                                intr_enabled = true;
                            }
                            else
                                LPC_UART3->IER &= ~(1 << 0);
                            break;
                        default:
                            return Unspecified_Error;
                    }
            /*
             * Start FIFO Queues and reset TX & RX Buffers
             */
                switch (device){
                    case U0:
                        LPC_UART0->FCR |= (1 << 0);
                        LPC_UART0->FCR |= ((1 << 1) | (1 << 2));
                        break;
                    case U1:
                        LPC_UART1->FCR |= (1 << 0);
                        LPC_UART1->FCR |= ((1 << 1) | (1 << 2));
                        break;
                    case U2:
                        LPC_UART2->FCR |= (1 << 0);
                        LPC_UART2->FCR |= ((1 << 1) | (1 << 2));
                        break;
                    case U3:
                        LPC_UART3->FCR |= (1 << 0);
                        LPC_UART3->FCR |= ((1 << 1) | (1 << 2));
                        break;
                    default:
                        return Unspecified_Error;
                }
            /*
             * Enable RX Status Line Interrupts
             */
                switch (device){
                    case U0:
                        LPC_UART0->IER |= (1 << 2);
                        break;
                    case U1:
                        LPC_UART1->IER |= (1 << 2);
                        break;
                    case U2:
                        LPC_UART2->IER |= (1 << 2);
                        break;
                    case U3:
                        LPC_UART3->IER |= (1 << 2);
                        break;
                    default:
                        return Unspecified_Error;
                }
            this_device = device;
            initialized = true;
            return Success;
        }
        default:
            return No_Device_Initialized;
    }
}

int LabUart::Transmit(char c)
{
    switch(this_device){
        case U0:
            while (!((LPC_UART0->LSR >> 5) & 1));
            LPC_UART0->THR = c;
            break;
        case U1:
            while (!((LPC_UART1->LSR >> 5) & 1));
            LPC_UART1->THR = c;
            break;
        case U2:
            while (!((LPC_UART2->LSR >> 5) & 1));
            LPC_UART2->THR = c;
            break;
        case U3:
            while (!((LPC_UART3->LSR >> 5) & 1));
            LPC_UART3->THR = c;
            break;
        default:
            return No_Device_Initialized;

    }
    return Success;
}

char LabUart::Receive()
{
    char c = NULL;
    if (intr_enabled){
        switch(this_device){
            case U0:
                xQueueReceive(u0_rx_queue, &c, DEQUEUE_TIMEOUT);
                break;
            case U1:
                xQueueReceive(u1_rx_queue, &c, DEQUEUE_TIMEOUT);
                break;
            case U2:
                xQueueReceive(u2_rx_queue, &c, DEQUEUE_TIMEOUT);
                break;
            case U3:
                xQueueReceive(u3_rx_queue, &c, DEQUEUE_TIMEOUT);
                break;
        }
    return c;
    }
    else{
        switch(this_device){
            case U0:
                while (!(LPC_UART0->LSR & 1));
                    c = LPC_UART0->RBR;
                break;
            case U1:
                while (!(LPC_UART1->LSR & 1));
                    c = LPC_UART1->RBR;
                break;
            case U2:
                while (!(LPC_UART2->LSR & 1));
                    c = LPC_UART2->RBR;
                break;
            case U3:
                while (!(LPC_UART3->LSR & 1));
                    c = LPC_UART3->RBR;
                break;
        }
    return c;
    }
}

void LabUart::uart0_rx_intr()
{
    char c = LPC_UART0->RBR;
    xQueueSend(u0_rx_queue, &c, ENQUEUE_TIMEOUT);
}

void LabUart::uart1_rx_intr()
{
    char c = LPC_UART1->RBR;
    xQueueSend(u1_rx_queue, &c, ENQUEUE_TIMEOUT);
}

void LabUart::uart2_rx_intr()
{
    char c = LPC_UART2->RBR;
    xQueueSend(u2_rx_queue, &c, ENQUEUE_TIMEOUT);
}

void LabUart::uart3_rx_intr()
{
    char c = LPC_UART3->RBR;
    xQueueSend(u3_rx_queue, &c, ENQUEUE_TIMEOUT);
}
