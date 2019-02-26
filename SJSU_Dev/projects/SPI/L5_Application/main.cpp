/*
 * main.cpp
 *
 *  Created on: Feb 24, 2019
 *      Author: Nick
 */

#include "SPI/LabSpi.hpp"
#include "tasks.hpp"
#include "stdio.h"
#include "FreeRTOS.h"

//#define PART1
#define PART2

#define select   true
#define deselect false

#define DATA_SIZE_SELECT    8
#define CLOCK_DIVISOR     100

uint8_t *mode1          = new uint8_t(1);
uint8_t *mode2          = new uint8_t(2);
SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
GPIO_0_1_2 CS           = GPIO_0_1_2(0,6);

void adesto_s(bool sel){
    sel ? CS.setLow() : CS.setHigh();
    return;
}

void vReadFlash(void *pvParameters){
    uint8_t mode = *((uint8_t*) pvParameters);
    LabSpi spi = LabSpi();
    spi.initialize(DATA_SIZE_SELECT, LabSpi::tSPI, CLOCK_DIVISOR);
    uint8_t manufacture_id;
    uint8_t device_id_1;
    uint8_t device_id_2;
    while (1){
        if (xSemaphoreTake(mutex, portMAX_DELAY)){
            adesto_s(select);
                spi.transfer(0x9F);
                manufacture_id = spi.transfer(0x9F);
                device_id_1    = spi.transfer(0xFF);
                device_id_2    = spi.transfer(0xFF);
            adesto_s(deselect);
            xSemaphoreGive(mutex);
            printf("Manufacture ID %d: %X\nDeviceID: %X\n\n", mode, manufacture_id, (device_id_1 << 8) + device_id_2);
        }
        vTaskDelay(1000);
    }
}

void vCheckStatus(void *pvParameters){
    LabSpi spi = LabSpi();
    spi.initialize(DATA_SIZE_SELECT, LabSpi::tSPI, CLOCK_DIVISOR);
    LabSpi::SPI_SR_t status;


    while(1){
        if (xSemaphoreTake(mutex,portMAX_DELAY)){
            adesto_s(select);
                spi.transfer(0xD7);
                status.byte1 = spi.transfer(0xD7);
                status.byte2 = spi.transfer(0xD7);
            adesto_s(deselect);
            xSemaphoreGive(mutex);

            //printf("SPI Flash Device Status:\n\tByte1: %X\n\tByte2: %X\n", status.byte1, status.byte2);



            if (status.READY)
                printf("Device is Ready.\n");
            else
                printf("Device is Busy with an internal operation.\n");

            if (status.COMP)
                printf("Main memory page data does not match buffer data.\n");
            else
                printf("Main memory page data matches buffer data.\n");

            if (status.DENSITY)
                printf("Density: 16-Mbit\n");
            else
                printf("Density: Unknown-Mbit\n");

            if (status.PROTECT)
                printf("Sector protection is enabled.\n");
            else
                printf("Sector protection is disabled.\n");

            if (status.PAGE_SIZE)
                printf("Device is configured for power of 2 binary page size (512 bytes).\n");
            else
                printf("Device is configured for standard DataFlash page size (528 bytes).\n");

            if (status.EPE)
                printf("Erase or program error detected.\n");
            else
                printf("Erase or program operation was successful.\n");

            if (status.SLE)
                printf("Sector Lockdown command is enabled.\n");
            else
                printf("Sector Lockdown command is disabled.\n");

            if (status.PS2)
                printf("A sector is program suspended while using Buffer 2.\n");
            else
                printf("No program operation has been suspended while using Buffer 2.\n");

            if (status.PS1)
                printf("A sector is program suspended while using Buffer 1.\n");
            else
                printf("No program operation has been suspended while using Buffer 1.\n");

            if (status.ES)
                printf("A sector is erase suspended.\n");
            else
                printf("No sectors are erase suspended.\n");

            printf("\n\n");


        }
        vTaskDelay(1000);

    }
}


int main(){
    CS.setAsOutput();
    CS.setHigh();
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
#ifdef PART1
    xTaskCreate(vReadFlash, "Read1", 1024, (void *)mode1, PRIORITY_LOW, NULL);
    xTaskCreate(vReadFlash, "Read2", 1024, (void *)mode2, PRIORITY_LOW, NULL);
#endif
#ifdef PART2
    xTaskCreate(vCheckStatus, "Status", 1024, NULL, PRIORITY_LOW, NULL);
#endif
    scheduler_start();
    return -1;
}



