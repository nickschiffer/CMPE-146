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

/**
 * @file main.cpp
 * @brief This is the application entry point.
 *          FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 *          @see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */

/*
 * Nickolas Schiffer #012279319
 * CMPE 146 LED Interrupts, Semaphores, and Lookup Tables
 */

#include <GPIO/GPIOInterrupt.hpp>
#include <GPIO/GPIO_0_1_2.hpp>
#include "tasks.hpp"
#include <stdio.h>
#include <stdint.h>
#include <uart0_min.h>



uint16_t rising_counter  = 0;
uint16_t falling_counter = 0;


void Eint3Handler(){
    GPIOInterrupt *interruptHandler = GPIOInterrupt::getInstance();
    interruptHandler->HandleInterrupt();
}

void callback_func_1(){
    printf("Rising Edge Called: %d\n\n", (int)rising_counter++);
}

void callback_func_2(){
    printf("Falling Edge Called: %d\n\n", (int)falling_counter++);
//    uart0_puts("Falling Edge Called\n\n");
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

  //Attach Handler to switches
  interruptHandler->AttachInterruptHandler(2, 0, (IsrPointer)callback_func_1, kRisingEdge);
  interruptHandler->AttachInterruptHandler(0, 0, (IsrPointer)callback_func_2, kFallingEdge);
//  interruptHandler->AttachInterruptHandler(2, 0, (IsrPointer)callback_func_3, kBothEdges);



  while (1)
  {

  }
  return EXIT_FAILURE;
}
