/*
 * CMPE 146: I2C Lab Main_Slave
 */

/**
 * @file
 * @brief This is the application entry point.
 */

#include <stdio.h>
#include "utilities.h"
#include "io.hpp"
#include <i2c2.hpp>
#include <tasks.hpp>
#include <GPIO/GPIOInterrupt.hpp>
#include <printf_lib.h>
#include <uart0_min.h>

volatile uint8_t buffer[256] = { 0 };

typedef enum {
    addition,
    subtraction,
    multiplication
} operation;

uint8_t operand_1 = 0, operand_2 = 0;

uint8_t result = 0;




void vReadBuffer(void *pvParameters){
    while(1){
        for (uint8_t i = 0; i < 10; i++){
            printf("Buffer %u: %X\n", i, buffer[i]);
        }
        puts("\n");
        vTaskDelay(1000);
    }
}


void vCalculate(void *pvParameters){
    while (1){
        //...do stuff
        operand_1 = buffer[1];
        operand_2 = buffer[2];

        switch (buffer[3]){
            case addition:
                result = operand_1 + operand_2;
                break;
            case subtraction:
                if (operand_1 >= operand_2){
                    result = operand_1 - operand_2;
                }
                else result = 0;
                break;
            case multiplication:
                if ((operand_1 < 16) && (operand_2 < 16)){
                    result = operand_1 * operand_2;
                }
                else result = 0;
                break;
            default:
                result = 0;
                break;
        }


        buffer[4] = result;

        //printf("op1: %u\nop2: %u\noperation: %u\nResult: %u\n\n\n",operand_1, operand_2, buffer[3], result);

        vTaskDelay(10);
    }
}



int main(void)
{


    I2C2& i2c = I2C2::getInstance();
    const uint8_t slaveAddr = 0xC0;

    i2c.initSlave(slaveAddr, &buffer[0], (size_t)sizeof(buffer));


    //xTaskCreate(vReadBuffer, "ReadBuf", 1024, NULL, PRIORITY_LOW, NULL);
    xTaskCreate(vCalculate, "Calc", 1024, NULL, PRIORITY_LOW, NULL);

    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    scheduler_start();

    return -1;
}
