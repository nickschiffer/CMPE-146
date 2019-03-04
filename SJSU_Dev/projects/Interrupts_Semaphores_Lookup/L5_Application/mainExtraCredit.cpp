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
 * @file
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */

/*
 * Nickolas Schiffer #012279319
 * CMPE 146 LED Switch
 */

#include "tasks.hpp"
#include "GPIO/GPIO_0_1_2.hpp"
#include "GPIO/GPIOInterrupt.hpp"
#include "LED_Display.hpp"
#include <stdio.h>


#define LOW  false
#define HIGH true
#define BINARY_COUNTER_TIME_INTERVAL 500
#define sw_number1 true
#define sw_number2 false

typedef bool sw_number;

/*
 * Global Variables used to keep various states
 */
volatile bool onboard_led_state    = true;
volatile bool external_led_state   = true;
volatile bool easter_egg           = false;
volatile uint32_t internal_counter = 0;
volatile uint32_t external_counter = 0;

SemaphoreHandle_t external_switch1_semaphore = NULL;
SemaphoreHandle_t external_switch2_semaphore = NULL;
SemaphoreHandle_t led_counter_semaphore      = NULL;

typedef struct params {
    uint8_t port;
    uint8_t pin;
    sw_number sw;
    bool onboard;
}params;


/*
 * Parameter Structs and subsequent pointers passed to the Tasks
 */

static const params external_led    = { .port = 2, .pin = 1, .sw = sw_number1, .onboard = false };
static const params internal_led    = { .port = 1, .pin = 8, .sw = sw_number2, .onboard = true };


static const params *pExternal_led     = &external_led;
static const params *pInternal_led     = &internal_led;

static const params *pExternal1 = params(2,1,sw_number1,false);

/*Easter Egg #1
 * Counts from 0 to 15 on both onboard LEDs and 7 seg digits.
 * Then rapidly counts to 99 on 7 seg displays and then back down to where
 * the 7 segs were previously. The state of the LEDs then return to the state that
 * they were in before the routine is called.
 * All other functionality is purposely interrupted while this routine completes.
 */
void vBinaryCounter(void *pvParameters)
{
    auto led3 = GPIO_0_1_2(1, 0);
    auto led2 = GPIO_0_1_2(1, 1);
    auto led1 = GPIO_0_1_2(1, 4);
    auto led0 = GPIO_0_1_2(1, 8);

    led0.setAsOutput();
    led1.setAsOutput();
    led2.setAsOutput();
    led3.setAsOutput();

    auto sw3 = GPIO_0_1_2(1, 15);

    sw3.setAsInput();

    LED_Display ledDisplay = LED_Display::getInstance();

    while(1){
        uint8_t counter = 0;

        if (sw3.getLevel()){

            easter_egg = true;
            uint16_t delay = 89;

            while (counter < 16){

                led0.set(!((bool)(counter & (1 << 0))));
                led1.set(!((bool)(counter & (1 << 1))));
                led2.set(!((bool)(counter & (1 << 2))));
                led3.set(!((bool)(counter & (1 << 3))));
                ledDisplay.setNumber(counter++);
                vTaskDelay(BINARY_COUNTER_TIME_INTERVAL);
            }


            while (counter < 100){
                ledDisplay.setNumber(counter++);
                led0.set(!((bool)(counter & (1 << 0))));
                led1.set(!((bool)(counter & (1 << 1))));
                led2.set(!((bool)(counter & (1 << 2))));
                led3.set(!((bool)(counter & (1 << 3))));
                vTaskDelay(delay--);
            }
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL);
            while (counter > ((internal_counter * 10) % 100) + (external_counter % 10)){
                ledDisplay.setNumber(counter--);
                led0.set(!((bool)(counter & (1 << 0))));
                led1.set(!((bool)(counter & (1 << 1))));
                led2.set(!((bool)(counter & (1 << 2))));
                led3.set(!((bool)(counter & (1 << 3))));
                vTaskDelay(delay++);
            }
            ledDisplay.setNumber(counter--);
            led0.setHigh();
            led1.setHigh();
            led2.setHigh();
            led3.setHigh();
            easter_egg = false;
            counter = 0;
            xSemaphoreGive(led_counter_semaphore);
        }
        else
            vTaskDelay(50);
    }
}

/*Easter Egg(?) #2
 * Counts number of internal LED Toggles on Left  7 seg
 * Counts number of external LED Toggles on right 7 seg
 */
void vLEDCounter(void *pvParameters)
{
    LED_Display ledDisplay = LED_Display::getInstance();
    bool success = ledDisplay.init();
    ledDisplay.clear();
    ledDisplay.setLeftDigit('H');
    ledDisplay.setRightDigit('I');
    vTaskDelay(1000);
    ledDisplay.clear();


    while(1){
        if (xSemaphoreTake(led_counter_semaphore, portMAX_DELAY)){
            while (easter_egg);
            ledDisplay.setNumber(((internal_counter * 10) % 100) + (external_counter % 10));
        }
    }


}

//Led Controller Task
void vControlLED(void *pvParameters)
{
    /* Get Parameter */
    params *param = (params*) pvParameters;
    /* Define Constants Here */

    /* Define Local Variables and Objects */

    uint8_t port = param->port;
    uint8_t pin  = param->pin;
    bool onboard = param->onboard;
    sw_number sw = param->sw;

    auto led = GPIO_0_1_2(port, pin);

    /* Initialization Code */

    led.setAsOutput();
    led.setHigh();
    while (1) {
        while (easter_egg);
        if (sw == sw_number1){
            if (xSemaphoreTake(external_switch1_semaphore, portMAX_DELAY)){
                led.set(!(onboard ? onboard_led_state : external_led_state));
                onboard ? onboard_led_state = !onboard_led_state : external_led_state = !external_led_state;
                onboard ? internal_counter++ : external_counter++;
                xSemaphoreGive(led_counter_semaphore);
            }
        }

        else if (sw == sw_number2){
            if (xSemaphoreTake(external_switch2_semaphore, portMAX_DELAY)){
                led.set(!(onboard ? onboard_led_state : external_led_state));
                onboard ? onboard_led_state = !onboard_led_state : external_led_state = !external_led_state;
                onboard ? internal_counter++ : external_counter++;
                xSemaphoreGive(led_counter_semaphore);
            }

        }

    }


}

void Eint3Handler(){
    GPIOInterrupt *interruptHandler = GPIOInterrupt::getInstance();
    interruptHandler->HandleInterrupt();
}

void vSwitchMode(){
    xSemaphoreGive(external_switch1_semaphore);
}

void vSemaphore2Supplier(){
    xSemaphoreGive(external_switch2_semaphore);
}




/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at scheduler_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */

int main(void)
{
    /**
     * A few basic tasks for this bare-bone system :
     *      1.  Terminal task provides gateway to interact with the board through UART terminal.
     *      2.  Remote task allows you to use remote control to interact with the board.
     *      3.  Wireless task responsible to receive, retry, and handle mesh network.
     *
     * Disable remote task if you are not using it.  Also, it needs SYS_CFG_ENABLE_TLM
     * such that it can save remote control codes to non-volatile memory.  IR remote
     * control codes can be learned by typing the "learn" terminal command.
     */
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    external_switch1_semaphore = xSemaphoreCreateBinary();
    external_switch2_semaphore = xSemaphoreCreateBinary();
    led_counter_semaphore      = xSemaphoreCreateBinary();



    GPIOInterrupt *gpio_interrupts = GPIOInterrupt::getInstance();
    gpio_interrupts->Initialize();
    gpio_interrupts->AttachInterruptHandler(2, 0, (IsrPointer)vSwitchMode, kRisingEdge);
    gpio_interrupts->AttachInterruptHandler(0, 0, (IsrPointer)vSemaphore2Supplier, kRisingEdge);
    isr_register(EINT3_IRQn, Eint3Handler);

    //Create Tasks

    xTaskCreate(vControlLED, "LED 1 Control", 1024, (void *) pExternal_led, PRIORITY_LOW, NULL);
    xTaskCreate(vControlLED, "LED 2 Control", 1024, (void *) pInternal_led, PRIORITY_LOW, NULL);
    xTaskCreate(vLEDCounter, "LED Counter", 1024, NULL, PRIORITY_LOW, NULL);
    xTaskCreate(vBinaryCounter, "Binary Counter", 1024, NULL, PRIORITY_LOW, NULL);


    scheduler_start(); ///< This shouldn't return
    return -1;
}
