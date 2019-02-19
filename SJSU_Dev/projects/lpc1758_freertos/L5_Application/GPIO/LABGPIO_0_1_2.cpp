#include "LabGPIO_0_1_2.hpp"
#include <LPC17xx.h>

LabGPIO_0_1_2::LabGPIO_0_1_2(uint8_t port, uint8_t pin)
{
    this->pin = pin;
    this->port = port;

}

void LabGPIO_0_1_2::setAsInput()
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

void LabGPIO_0_1_2::setAsOutput()
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

void LabGPIO_0_1_2::setDirection(bool output)
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

void LabGPIO_0_1_2::setHigh()
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

void LabGPIO_0_1_2::setLow()
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

void LabGPIO_0_1_2::set(bool high)
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

bool LabGPIO_0_1_2::getLevel()
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

