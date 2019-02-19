/*
 * Nickolas Schiffer #012279319
 * CMPE 146 LED Interrupts, Semaphores, and Lookup Tables
 */


#include <GPIO/GPIO_0_1_2.hpp>
#include <GPIO/GPIOInterrupt.hpp>
#include <LPC17xx.h>
#include <scheduler_task.hpp>
#include <stdio.h>
#include "tasks.hpp"



uint16_t rising_counter  = 0;
uint16_t falling_counter = 0;

SemaphoreHandle_t rising_semaphore = NULL;
SemaphoreHandle_t falling_semaphore = NULL;


void vRising_Semaphore_Supplier(){
    xSemaphoreGive(rising_semaphore);
}

void vFalling_Semaphore_Supplier(){
    xSemaphoreGive(falling_semaphore);
}


void Eint3Handler(){
    GPIOInterrupt *interruptHandler = GPIOInterrupt::getInstance();
    interruptHandler->HandleInterrupt();
}

void callback_func_1(void *pvParameters){
    while (1){
        if (xSemaphoreTake(rising_semaphore, portMAX_DELAY))
            printf("Rising Edge Called: %d\n\n", (int)rising_counter++);
    }
}

void callback_func_2(void *pvParameters){
    while (1){
        if (xSemaphoreTake(falling_semaphore, portMAX_DELAY))
            printf("Falling Edge Called: %d\n\n", (int)falling_counter++);
    }
}

void callback_func_3(){
    printf("Both Edges Called\n\n");
}



int main(void)
{

  //Initialize GPIO ports as inputs
   auto sw1 = GPIO_0_1_2(2, 0);
   auto sw2 = GPIO_0_1_2(0, 0);

   sw1.setAsInput();
   sw2.setAsInput();

  // Initialize GPIOInterrupt Instance
  GPIOInterrupt *interruptHandler =  GPIOInterrupt::getInstance();
  interruptHandler->Initialize();
  isr_register(EINT3_IRQn, Eint3Handler);

  rising_semaphore  = xSemaphoreCreateBinary();
  falling_semaphore = xSemaphoreCreateBinary();

  //Attach Handler to switches
  interruptHandler->AttachInterruptHandler(2, 0, (IsrPointer)vRising_Semaphore_Supplier,  kRisingEdge);
  interruptHandler->AttachInterruptHandler(0, 0, (IsrPointer)vFalling_Semaphore_Supplier, kFallingEdge);

  scheduler_add_task(new terminalTask(PRIORITY_HIGH));
  xTaskCreate(callback_func_1, "Rising Task" , 1024, NULL, PRIORITY_LOW, NULL);
  xTaskCreate(callback_func_2, "Falling Task", 1024, NULL, PRIORITY_LOW, NULL);

  scheduler_start();
  return -1;
}
