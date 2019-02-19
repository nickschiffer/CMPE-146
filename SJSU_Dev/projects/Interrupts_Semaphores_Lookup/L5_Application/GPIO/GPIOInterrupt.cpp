/*
 * GPIOInterrupt.cpp
 *
 *  Created on: Feb 16, 2019
 *      Author: Nick Schiffer
 */

#include "GPIOInterrupt.hpp"

GPIOInterrupt::GPIOInterrupt()
{
}

GPIOInterrupt* GPIOInterrupt::getInstance()
{

    static GPIOInterrupt instance;
    return &instance;
}

void GPIOInterrupt::Initialize()
{
    NVIC_EnableIRQ(EINT3_IRQn);
}

bool GPIOInterrupt::AttachInterruptHandler(uint8_t port, uint32_t pin, IsrPointer pin_isr, InterruptCondition condition)
{
        switch(port){
            case 0:{
                //check if valid port 0 pin
                if ((((pin >= 0) && (pin <= 11)) || ((pin >= 15) && (pin <= 30)))){

                    //store ISR pointer in map
                    pin_isr_map[0][pin] = pin_isr;



                    switch(condition){
                        case kRisingEdge:{
                            LPC_GPIOINT->IO0IntEnR |=  (1 << pin);
                            LPC_GPIOINT->IO0IntEnF &= ~(1 << pin);
                        }
                            break;
                        case kFallingEdge:{
                            LPC_GPIOINT->IO0IntEnR &= ~(1 << pin);
                            LPC_GPIOINT->IO0IntEnF |=  (1 << pin);
                        }
                            break;
                        case kBothEdges:{
                            LPC_GPIOINT->IO0IntEnR |= (1 << pin);
                            LPC_GPIOINT->IO0IntEnF |= (1 << pin);
                        }
                            break;
                        default:
                            return 1;
                    }

                }
                else
                    return 1;
                break;
            }
            case 2:{
                //check if valid port 2 pin
                if (((pin >= 0) && (pin <= 13))){

                    pin_isr_map[1][pin] = pin_isr;

                    switch(condition){
                        case kRisingEdge:{
                            LPC_GPIOINT->IO2IntEnR |=  (1 << pin);
                            LPC_GPIOINT->IO2IntEnF &= ~(1 << pin);
                        }
                            break;
                        case kFallingEdge:{
                            LPC_GPIOINT->IO2IntEnR &= ~(1 << pin);
                            LPC_GPIOINT->IO2IntEnF |=  (1 << pin);
                        }
                            break;
                        case kBothEdges:{
                            LPC_GPIOINT->IO2IntEnR |= (1 << pin);
                            LPC_GPIOINT->IO2IntEnF |= (1 << pin);
                        }
                            break;
                        default:
                            return 1;
                    }
                }
                else
                    return 1;
                break;
            }
            default:
                return 1;
        }
        return 0;
}



void GPIOInterrupt::HandleInterrupt()
{
    /*
     * Check to see if there is an interrupt on any of the pins
     * (any nonzero value evaluates to "true")
     */
    if (LPC_GPIOINT->IO0IntStatR){
        uint8_t counter = 0;
        /*
         * Iterate through the bits by shifting right until the
         * register evaluates to 0:false or all the pins have been checked
         */
        while ((counter <= 30) && (LPC_GPIOINT->IO0IntStatR >> counter)){
            /*
             * If the rightmost digit is a 1 (odd) we've hit an interrupt
             */
            if ((LPC_GPIOINT->IO0IntStatR >> counter) & 1){
                /*
                 * If the pin_isr_map has an entry for this pin (!= nullptr), we know we've
                 * established a handler for this interrupt previously. We then clear this interrupt
                 * by writing a 1 to the corresponding bit position. Now we can execute the
                 * callback isr pointer stored in the pin_isr_map.
                 */
                if (pin_isr_map[0][counter] != nullptr){
                    pin_isr_map[0][counter]();
                    LPC_GPIOINT->IO0IntClr = (1 << counter);
                }
            }
            counter++;
        }

    }

    if (LPC_GPIOINT->IO0IntStatF){
        uint8_t counter = 0;
        while ((counter <= 30) && (LPC_GPIOINT->IO0IntStatF >> counter)){
            if ((LPC_GPIOINT->IO0IntStatF >> counter) & 1){
                if (pin_isr_map[0][counter] != nullptr){
                    pin_isr_map[0][counter]();
                    LPC_GPIOINT->IO0IntClr = (1 << counter);
                }
            }
            counter++;
        }
    }

    if (LPC_GPIOINT->IO2IntStatR){
        uint8_t counter = 0;
        while ((counter <= 13) && (LPC_GPIOINT->IO2IntStatR >> counter)){
            if ((LPC_GPIOINT->IO2IntStatR >> counter) & 1){
                if (pin_isr_map[1][counter] != nullptr){
                    pin_isr_map[1][counter]();
                    LPC_GPIOINT->IO2IntClr = (1 << counter);
                }
            }
            counter++;
        }

    }

    if (LPC_GPIOINT->IO2IntStatF){
        uint8_t counter = 0;
        while ((counter <= 13) && (LPC_GPIOINT->IO2IntStatF >> counter)){
           if ((LPC_GPIOINT->IO2IntStatF >> counter) & 1){
               if (pin_isr_map[1][counter] != nullptr){
                   pin_isr_map[1][counter]();
                   LPC_GPIOINT->IO2IntClr = (1 << counter);
               }
           }
           counter++;
       }

    }
}

GPIOInterrupt::~GPIOInterrupt()
{
}

