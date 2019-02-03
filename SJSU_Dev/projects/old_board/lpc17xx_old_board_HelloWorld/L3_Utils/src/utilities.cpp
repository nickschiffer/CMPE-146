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

#include <stdio.h>
#include <string.h>

#include "utilities.h"
#include "LPC17xx.h"
#include "sys_config.h" // TIMER0_US_PER_TICK
#include "memory.h"

#include "rtc.h"
#include "ff.h"
#include "boot_type.h"
#include "fault_registers.h"

#include <stdarg.h>
#include "uart0_min.h"


/**
 * Delays in microseconds
 * @param delayMicroSec The delay in microseconds
 */
void delay_us(unsigned int delayMicroSec)
{
    const unsigned int ticksForThisDelay = delayMicroSec / TIMER0_US_PER_TICK;
    const unsigned int targetTick = LPC_TIM0->TC + ticksForThisDelay;

    // Take care of TC overflow case by allowing TC value to go beyond
    // targetTick, and then the next loop will handle the rest.
    if(targetTick < LPC_TIM0->TC) {
        while(LPC_TIM0->TC > targetTick) {
            ;
        }
    }

    // Wait until the TC value reaches targetTick
    while(LPC_TIM0->TC < targetTick) {
        ;
    }
}

/**
 * Delays in milliseconds
 * @param delayMilliSec The delay in milliseconds.
 */
void delay_ms(unsigned int delayMilliSec)
{
    /**
     * If FreeRTOS is running, we should sleep the calling task instead
     * of polling using the full CPU.
     */
    if(isFreeRtosRunning())
    {
        vTaskDelay(OS_MS(delayMilliSec));
    }
    else
    {
        delay_us(1000 * delayMilliSec);
    }
}

char isFreeRtosRunning()
{
    return (taskSCHEDULER_RUNNING == xTaskGetSchedulerState());
}

void printMemoryInfo(char* pBuff)
{
    MemoryInfoType info = getMemoryInfo();
    sprintf(pBuff,
            "Memory Information:\n"
            "Global Used   : %5u\n"
            "Heap   Used   : %5u\n"
            "Heap Avail.   : %5u\n"
            "System Avail. : %5u\n",
            info.globalUsed, info.heapUsed, info.heapAvailable, info.systemAvailable);
}

void logBootInfo(const char *pExtraInfo)
{
    const char filename[] = "0:boot.csv";
    char logMsg[128] = { 0 };
    char dateTime[24] = { 0 };

    rtc_getDateTimeString(dateTime);

    if(rebootType_watchdogRecover == getRebootCause()) {
        sprintf(logMsg, "%s, BAD BOOT (%s): PC: 0x%08X LR: 0x%08X PSR: 0x%08X\n",
                        dateTime, pExtraInfo,
                        (unsigned int)FAULT_PC, (unsigned int)FAULT_LR, (unsigned int)FAULT_PSR);
    }
    else {
        sprintf(logMsg, "%s, NORMAL BOOT (%s)\n", dateTime, pExtraInfo);
    }

    FIL file;
    unsigned int bytesWritten = 0;
    if (FR_OK == f_open(&file, filename, FA_OPEN_ALWAYS | FA_WRITE))
    {
        if(FR_OK == f_lseek(&file, f_size(&file))) {
            f_write(&file, logMsg, strlen(logMsg), &bytesWritten);
        }
        f_close(&file);
    }

    if(strlen(logMsg) != bytesWritten) {
        puts("Error writing boot info");
    }
}
