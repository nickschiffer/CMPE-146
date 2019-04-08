/*
 * CMPE 146: I2C Lab main_master.cpp
 */

/**
 * @file
 * @brief This is the application entry point.
 */
#include <stdio.h>
#include "utilities.h"
#include "io.hpp"
#include <tasks.hpp>
#include "i2c2.hpp"
#include "time.h"
void vCalculate(void *pvParameters){
    uint8_t op_1, op_2, opr, result;
    I2C2& i2c = I2C2::getInstance(); // Get I2C driver instance
    const uint8_t slaveAddr = 0xC0;  // Pick any address other than an existing one at i2c2.hpp
    while (1){
    uint8_t arr[3] = { 0 };
        op_1 = rand() % 16;
        op_2 = rand() % 16;
        opr  = rand() % 3;
        arr[0] = op_1;
        arr[1] = op_2;
        arr[2] = opr;
        i2c.writeRegisters(slaveAddr, 0x01, arr, 3);
        vTaskDelay(500);
        result = i2c.readReg(0xc0, 0x04);
        switch (opr){
            case 0:
                if ((op_1 + op_2) == result){
                    printf("%u + %u = %u\n", op_1, op_2, result);
                }
                else {
                    printf("error: got %u + %u = %u\nexpected %u + %u = %u\n", op_1, op_2, result, op_1, op_2, op_1 + op_2);
                }
                break;
            case 1:
                if (op_1 > op_2){
                    if (result == (op_1 - op_2)){
                        printf("%u - %u = %u\n", op_1, op_2, result);
                    }
                    else {
                        printf("error: got %u - %u = %u\nexpected %u - %u = %u\n", op_1, op_2, result, op_1, op_2, op_1 - op_2);
                    }
                }
                break;
            case 2:
                if ((op_1 * op_2) == result){
                    printf("%u * %u = %u\n", op_1, op_2, result);
                }
                else {
                    printf("error: got %u * %u = %u\nexpected %u * %u = %u\n", op_1, op_2, result, op_1, op_2, op_1 * op_2);
                }
                break;
        }
    }
}
int main(void)
{
    srand(time(NULL));
    xTaskCreate(vCalculate, "Calc", 1024, NULL, PRIORITY_LOW, NULL);
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    scheduler_start();
    return -1;
}
