/*
 *  Nickolas Schiffer
 *  CMPE 146 S19
 *  Lab: UART: part0
 */

/**
 * @file
 * @brief This is the application entry point.
 */

#include <stdio.h>
#include <LPC17xx.h>
#include "utilities.h"
#include <tasks.hpp>
#include <switches.hpp>
#include <LED_Display.hpp>
#include <UART/UART_0_1_2_3.hpp>


#define LOW  false
#define HIGH true

enum operation {
    add,
    sub,
    mult
};
uint8_t dig1 = 0;
uint8_t dig2 = 0;
operation op = add;


SemaphoreHandle_t rx_sem = NULL;

void vALU(void *pvParameters){
    auto led = LED_Display::getInstance();
    auto uart3 = LabUart();
    uart3.Initialize(LabUart::U3, LabUart::b38400, true, LabUart::f8_bit);
    uint8_t d1, d2, result = 0;
    operation op_recv;
    while(1){
        d1 = (uint8_t)uart3.Receive();
        d2 = (uint8_t)uart3.Receive();
        op_recv = (operation)uart3.Receive();
        printf("ALU recv d1: %u, d2: %u, op: %u\n", d1, d2, op_recv);
        switch(op_recv){
            case add:
                result = d1 + d2;
                break;
            case sub:
                if (d1 < d2){
                    printf("no negative results please\n");
                    result = 0;
                    break;
                }
                result = d1 - d2;
                break;
            case mult:
                result = d1 * d2;
                break;
        }
        led.setNumber(result);
        uart3.Transmit((char)(result / 10));
        uart3.Transmit((char)(result % 10));
        printf("ALU result: %u\n", result);

    }
}

void vControl_Unit(void *pvParameters){
    auto led = LED_Display::getInstance();

    LabUart uart2 = LabUart();
    uart2.Initialize(LabUart::U2, LabUart::b38400, true, LabUart::f8_bit);
    uint8_t result1, result2;
    while(1){
        if (xSemaphoreTake(rx_sem, portMAX_DELAY)){
            printf("started\n");
            uart2.Transmit((char)dig1);
            uart2.Transmit((char)dig2);
            uart2.Transmit((char)op);

            result1 = (uint8_t)uart2.Receive();
            result2 = (uint8_t)uart2.Receive();
            printf("received: %d, %d\n", result1, result2);
            led.setNumber((result1 * 10) + result2);
            printf("CU Result %d\n",(result1 * 10) + result2);
            vTaskDelay(500);
        }

    }
}

void vInterfaceControl(void *pvParamters){
    auto sw = Switches::getInstance();
    auto led = LED_Display::getInstance();
    led.init();
    led.clear();
    sw.init();
    bool sw_prev_states[4] = {HIGH};
    bool sw_current_states[4] = {HIGH};

    while (1){
        for (int i = 0; i < 4; i++){
            sw_prev_states[i] = sw_current_states[i];
            sw_current_states[i] = sw.getSwitch(i + 1);
            if ((sw_current_states[i] == HIGH) && (sw_prev_states[i] == LOW)){
                switch(i){
                    case 0:
                        dig1 = (dig1 + 1) % 10;
                        printf("dig1: %d\n",dig1);
                        led.setNumber((dig1 * 10) + dig2);
                        break;
                    case 1:
                        dig2 = (dig2 + 1) % 10;
                        printf("dig2: %d\n",dig2);
                        led.setNumber((dig1 * 10) + dig2);
                        break;
                    case 2:
                        switch(op){
                            case add:
                                op = sub;
                                break;
                            case sub:
                                op = mult;
                                break;
                            case mult:
                                op = add;
                                break;
                        }
                        printf("op: %d\n",op);
                        break;
                    case 3:
                        xSemaphoreGive(rx_sem);
                        vTaskDelay(500);
                        break;

                }
            }
        }
    }
}

int main(){

    rx_sem = xSemaphoreCreateBinary();

    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    xTaskCreate(vInterfaceControl,"InterfaceControl",1000, NULL,PRIORITY_LOW, NULL);
    xTaskCreate(vControl_Unit,"CU",1000, NULL,PRIORITY_LOW, NULL);
    xTaskCreate(vALU,"ALU",1000, NULL,PRIORITY_LOW, NULL);

    scheduler_start();
    return -1;
}
