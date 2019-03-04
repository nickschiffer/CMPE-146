/*
 * adcDriver.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: Nickolas Schiffer
 */

#include "adcDriver.hpp"

LabAdc::LabAdc()
{
}

void LabAdc::AdcInitBurstMode()
{
    /*
     * Set pin 0.25 to ADC0.2
     */
    //LPC_PINCON->PINSEL1 |= (1 << 18);

    /*
     * Initialize ADC Power
     */
    LPC_SC->PCONP |= (1 << pconp_adc);
    /*
     * Set clock divider (should be <= 13MHz) -> 48Mhz / 4 = 12Mhz: bits 15:8
     */
    LPC_ADC->ADCR |= (4 << CLOCK_DIV);
    /*
     * Set START bits to 000
     */
    LPC_ADC->ADCR &= ~(7 << 24);
    /*
     * Enable Burst Mode: bit 16
     */
    LPC_ADC->ADCR |= (1 << 16);
    /*
     * Enable ADC Operational State: pin 21
     */
    LPC_ADC->ADCR |= (1 << 21);
}

void LabAdc::AdcSelectPin(Pin pin)
{
    switch(pin){
        case k0_25:
            LPC_PINCON->PINSEL1 |= (1 << p0_25);
            LPC_ADC->ADCR       |= (1 << k0_25);
            break;
        case k0_26:
            LPC_PINCON->PINSEL1 |= (1 << p0_26);
            LPC_ADC->ADCR       |= (1 << k0_26);
            break;
        case k1_30:
            LPC_PINCON->PINSEL3 |= (1 << p1_30);
            LPC_ADC->ADCR       |= (1 << k1_30);
            break;
        case k1_31:
            LPC_PINCON->PINSEL3 |= (1 << p1_31);
            LPC_ADC->ADCR       |= (1 << k1_31);
            break;
        default:
            break;
    }
}

float LabAdc::ReadAdcVoltageByChannel(ADC_Channel channel)
{
    float voltage = 0;
    switch (channel){
        case channel_0:
            voltage = (uint16_t)LPC_ADC->ADDR0;
            break;
        case channel_1:
            voltage = (uint16_t)LPC_ADC->ADDR1;
            break;
        case channel_2:
            voltage = (uint16_t)LPC_ADC->ADDR2;
            break;
        case channel_3:
            voltage = (uint16_t)LPC_ADC->ADDR3;
            break;
        case channel_4:
            voltage = (uint16_t)LPC_ADC->ADDR4;
            break;
        case channel_5:
            voltage = (uint16_t)LPC_ADC->ADDR5;
            break;
        case channel_6:
            voltage = (uint16_t)LPC_ADC->ADDR6;
            break;
        case channel_7:
            voltage = (uint16_t)LPC_ADC->ADDR7;
            break;
        default:
            return -1;
    }

    voltage /= (float)0xFFFF;
    voltage *= (float)VREF;
    return voltage;
}
