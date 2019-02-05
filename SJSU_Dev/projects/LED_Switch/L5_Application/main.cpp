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
#include "GPIO/LabGPIO_0_1_2.hpp"
#include "LED_Display.hpp"
#include <stdio.h>


#define LOW  false
#define HIGH true
#define BINARY_COUNTER_TIME_INTERVAL 500

/*
 * Global Variables used to keep various states
 */
volatile bool onboard_led_state    = false;
volatile bool external_led_state   = false;
volatile bool easter_egg           = false;
volatile uint32_t internal_counter = 0;
volatile uint32_t external_counter = 0;

typedef struct params {
    uint8_t port;
    uint8_t pin;
    bool onboard;
};

/*
 * Parameter Structs and subsequent pointers passed to the Tasks
 */
static const params onboard_switch  = { .port = 1, .pin = 9, .onboard = true };
static const params onboard_led     = { .port = 1, .pin = 0, .onboard = true };
static const params external_switch = { .port = 2, .pin = 1, .onboard = false };
static const params external_led    = { .port = 2, .pin = 0, .onboard = false };

static const params *pOnboard_switch  = &onboard_switch;
static const params *pOnboard_led     = &onboard_led;
static const params *pExternal_switch = &external_switch;
static const params *pExternal_led    = &external_led;

/*Easter Egg #1
 * Counts from 0 to 15 on both onboard LEDs and 7 seg digits.
 * Then rapidly counts to 99 on 7 seg displays and then back down to where
 * the 7 segs were previously. The state of the LEDs then return to the state that
 * they were in before the routine is called.
 * All other functionality is purposely interrupted while this routine completes.
 */
void vBinaryCounter(void *pvParameters)
{
    auto led3 = LabGPIO_0_1_2(1, 0);
    auto led2 = LabGPIO_0_1_2(1, 1);
    auto led1 = LabGPIO_0_1_2(1, 4);
    auto led0 = LabGPIO_0_1_2(1, 8);

    led0.setAsOutput();
    led1.setAsOutput();
    led2.setAsOutput();
    led3.setAsOutput();

    auto sw3 = LabGPIO_0_1_2(1, 15);

    sw3.setAsInput();

    LED_Display ledDisplay = LED_Display::getInstance();

    while(1){
        uint8_t counter = 0;

        if (sw3.getLevel()){ //3210
            easter_egg = true;
            ledDisplay.setNumber(counter++);
            led3.setHigh();
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //0001
            led0.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //0010
            led0.setHigh();
            led1.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //0011
            led0.setLow();
            led1.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //0100
            led0.setHigh();
            led1.setHigh();
            led2.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //0101
            led0.setLow();
            led1.setHigh();
            led2.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //0110
            led0.setHigh();
            led1.setLow();
            led2.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //0111
            led0.setLow();
            led1.setLow();
            led2.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //1000
            led0.setHigh();
            led1.setHigh();
            led2.setHigh();
            led3.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //1001
            led0.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //1010
            led0.setHigh();
            led1.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //1011
            led0.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //1100
            led0.setHigh();
            led1.setHigh();
            led2.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //1101
            led0.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //1110
            led0.setHigh();
            led1.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL); //1111
            led0.setLow();
            ledDisplay.setNumber(counter++);
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL);
            led0.setHigh();
            led1.setHigh();
            led2.setHigh();
            led3.setHigh();
            vTaskDelay(BINARY_COUNTER_TIME_INTERVAL);
            led0.setLow();
            led1.setLow();
            led2.setLow();
            led3.setLow();
            while (counter < 100){
                ledDisplay.setNumber(counter++);
                vTaskDelay(5);
            }
            while (counter > ((internal_counter * 10) % 100) + (external_counter % 10)){
                ledDisplay.setNumber(counter--);
                vTaskDelay(5);
            }
            ledDisplay.setNumber(counter--);
            led0.setHigh();
            led1.setHigh();
            led2.setHigh();
            led3.setHigh();
            easter_egg = false;
            counter = 0;
        }
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

    success ? printf("LED Success.\n\n") : printf("LED Failure\n\n");

    while(1){
        while (easter_egg);
        ledDisplay.setNumber(((internal_counter * 10) % 100) + (external_counter % 10));
        vTaskDelay(30);
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

    auto led = LabGPIO_0_1_2(port, pin);

    /* Initialization Code */
    switch (port) {
        case 0: {
            if ((pin >= 0) && (pin <= 11)){
                LPC_PINCON->PINSEL0 &= ~(3 << (pin * 2));
            }
            else if (pin == 15){
                LPC_PINCON->PINSEL0 &= ~(3 << 30);
            }
            else if ((pin >= 16) && (pin <= 30)){
                LPC_PINCON->PINSEL1 &= ~(3 << ((pin - 16) * 2));
            }
            else break;
            break;
        }
        case 1: {
            if ((pin >= 0) && (pin <= 1)){
                LPC_PINCON->PINSEL2 &= ~(3 << (pin * 2));
            }
            else if (pin == 4){
                LPC_PINCON->PINSEL2 &= ~(3 << 8);
            }
            else if ((pin >= 8) && (pin <= 10)){
                LPC_PINCON->PINSEL2 &= ~(3 << (pin * 2));
            }
            else if ((pin >= 14) && (pin <= 15)){
                LPC_PINCON->PINSEL2 &= ~(3 << (pin * 2));
            }
            else if ((pin >= 16) && (pin <= 31)){
                LPC_PINCON->PINSEL3 &= ~(3 << ((pin - 16) * 2));
            }
            else break;
            break;
        }
        case 2: {
            if ((pin >= 0) && (pin <= 13)){
                LPC_PINCON->PINSEL4 &= ~(3 << (pin * 2));
            }
            else break;
            break;
        }

        default:
            break;
    }

    led.setAsOutput();
    led.setLow();
    printf("Starting Onboard-LED\n\n");
    while (1) {
        while (easter_egg);
        led.set(!(onboard ? onboard_led_state : external_led_state));
    }

}

//Switch Reading Task
void vReadSwitch(void *pvParameters)
{
    /* Get Parameter */
    params *param = (params*) pvParameters;
    /* Define Constants Here */

    /* Define Local Variables and Objects */

    uint8_t port = param->port;
    uint8_t pin  = param->pin;
    bool onboard = param->onboard;

    bool prev_state    = LOW;
    bool current_state = LOW;

    auto sw = LabGPIO_0_1_2(port, pin);

    /* Initialization Code */
    //GPIO Selection, gets kind of messy
    switch (port) {
        case 0: {
            if ((pin >= 0) && (pin <= 11)){
                LPC_PINCON->PINSEL0 &= ~(3 << (pin * 2));
            }
            else if (pin == 15){
                LPC_PINCON->PINSEL0 &= ~(3 << 30);
            }
            else if ((pin >= 16) && (pin <= 30)) {
                LPC_PINCON->PINSEL1 &= ~(3 << ((pin - 16) * 2));
            }
            else break;
            break;
        }
        case 1: {
            if ((pin >= 0) && (pin <= 1)){
                LPC_PINCON->PINSEL2 &= ~(3 << (pin * 2));
            }
            else if (pin == 4){
                LPC_PINCON->PINSEL2 &= ~(3 << 8);
            }
            else if ((pin >= 8) && (pin <= 10)){
                LPC_PINCON->PINSEL2 &= ~(3 << (pin * 2));
            }
            else if ((pin >= 14) && (pin <= 15)){
                LPC_PINCON->PINSEL2 &= ~(3 << (pin * 2));
            }
            else if ((pin >= 16) && (pin <= 31)){
                LPC_PINCON->PINSEL3 &= ~(3 << ((pin - 16) * 2));
            }
            else break;
            break;
        }
        case 2: {
            if ((pin >= 0) && (pin <= 13)){
                LPC_PINCON->PINSEL4 &= ~(3 << (pin * 2));
            }
            else break;
            break;
        }

        default:
            break;
    }

    sw.setAsInput();

    while (1) {
        /* Insert Loop Code */
        prev_state = current_state;
        current_state = sw.getLevel();

        while (easter_egg);

        if ((current_state == LOW) && (prev_state == HIGH)) {
            if (onboard) {
                onboard_led_state = !onboard_led_state;
                onboard_led_state ? printf("On-Board LED On\n\n") : printf("On-Board LED Off.\n\n");
                internal_counter++;
            }
            else {
                external_led_state = !external_led_state;
                onboard_led_state ? printf("External LED On\n\n") : printf("External LED Off.\n\n");
                external_counter++;
            }
        }

    }
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

    //Establish Parameters for Switch and LED Controllers

    //Create Tasks

    xTaskCreate(vControlLED, "internal LED Control", 1024, (void *) pOnboard_led, PRIORITY_LOW, NULL);
    xTaskCreate(vReadSwitch, "internal Switch Read", 1024, (void *) pOnboard_switch, PRIORITY_LOW, NULL);
    xTaskCreate(vControlLED, "external LED Control", 1024, (void *) pExternal_led, PRIORITY_LOW, NULL);
    xTaskCreate(vReadSwitch, "external Switch Read", 1024, (void *) pExternal_switch, PRIORITY_LOW, NULL);
    xTaskCreate(vLEDCounter, "LED Counter", 1024, NULL, PRIORITY_LOW, NULL);
    xTaskCreate(vBinaryCounter, "Binary Counter", 1024, NULL, PRIORITY_LOW, NULL);


    scheduler_start(); ///< This shouldn't return
    return -1;
}
