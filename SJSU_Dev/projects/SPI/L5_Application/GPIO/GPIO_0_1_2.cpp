/*
 * Nickolas Schiffer #012279319
 * CMPE 146 LED Switch
 */




#include "GPIO_0_1_2.hpp"
#include <LPC17xx.h>

GPIO_0_1_2::GPIO_0_1_2(uint8_t port, uint8_t pin)
{
    this->pin = pin;
    this->port = port;

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
        initialized = true;
        return;

}

void GPIO_0_1_2::setAsInput()
{

    switch (port) {
        case 0:
            LPC_GPIO0->FIODIR &= ~(1 << pin);
            break;
        case 1:
            LPC_GPIO1->FIODIR &= ~(1 << pin);
            break;
        case 2:
            LPC_GPIO2->FIODIR &= ~(1 << pin);
            break;
        default:
            break;
    }

    return;
}

void GPIO_0_1_2::setAsOutput()
{
    switch (port) {
        case 0:
            LPC_GPIO0->FIODIR |= (1 << pin);
            break;
        case 1:
            LPC_GPIO1->FIODIR |= (1 << pin);
            break;
        case 2:
            LPC_GPIO2->FIODIR |= (1 << pin);
            break;
        default:
            break;
    }

    return;

}

void GPIO_0_1_2::setDirection(bool output)
{
    if (output) {
        switch (port) {
            case 0:
                LPC_GPIO0->FIODIR |= (1 << pin);
                break;
            case 1:
                LPC_GPIO1->FIODIR |= (1 << pin);
                break;
            case 2:
                LPC_GPIO2->FIODIR |= (1 << pin);
                break;
            default:
                break;
        }
    }
    else {
        switch (port) {
            case 0:
                LPC_GPIO0->FIODIR &= ~(1 << pin);
                break;
            case 1:
                LPC_GPIO1->FIODIR &= ~(1 << pin);
                break;
            case 2:
                LPC_GPIO2->FIODIR &= ~(1 << pin);
                break;
            default:
                break;
        }

    }

}

void GPIO_0_1_2::setHigh()
{
    switch (port) {
        case 0:
            LPC_GPIO0->FIOSET = (1 << pin);
            break;
        case 1:
            LPC_GPIO1->FIOSET = (1 << pin);
            break;
        case 2:
            LPC_GPIO2->FIOSET = (1 << pin);
            break;
        default:
            break;
    }
    return;
}

void GPIO_0_1_2::setLow()
{
    switch (port) {
        case 0:
            LPC_GPIO0->FIOCLR = (1 << pin);
            break;
        case 1:
            LPC_GPIO1->FIOCLR = (1 << pin);
            break;
        case 2:
            LPC_GPIO2->FIOCLR = (1 << pin);
            break;
        default:
            break;
    }
    return;
}

void GPIO_0_1_2::set(bool high)
{
    switch (port) {
        case 0:
            high ? LPC_GPIO0->FIOSET = (1 << pin) : LPC_GPIO0->FIOCLR = (1 << pin);
            break;
        case 1:
            high ? LPC_GPIO1->FIOSET = (1 << pin) : LPC_GPIO1->FIOCLR = (1 << pin);
            break;
        case 2:
            high ? LPC_GPIO2->FIOSET = (1 << pin) : LPC_GPIO2->FIOCLR = (1 << pin);
            break;
        default:
            break;
    }
    return;
}

bool GPIO_0_1_2::getLevel()
{
    bool level;

    switch (port) {
        case 0:
            level = (bool) (LPC_GPIO0->FIOPIN & (1 << pin));
            break;
        case 1:
            level = (bool) (LPC_GPIO1->FIOPIN & (1 << pin));
            break;
        case 2:
            level = (bool) (LPC_GPIO2->FIOPIN & (1 << pin));
            break;
        default:
            level = false;
            break;

    }

    return level;

}

GPIO_0_1_2::~GPIO_0_1_2()
{
}
