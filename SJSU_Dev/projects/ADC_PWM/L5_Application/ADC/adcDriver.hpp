/*
 * adcDriver.hpp
 *
 *  Created on: Mar 2, 2019
 *      Author: Nick Schiffer
 */

#ifndef ADCDRIVER_HPP_
#define ADCDRIVER_HPP_


#include <LPC17xx.h>
#include "io.hpp"

#define CLOCK_DIV       4
#define ADC_PIN_NUMBER 18
#define VREF            3.3

class LabAdc
{
public:
    enum Pin
    {
        k0_25 = 2,       // AD0.2 <-- Light Sensor -->
        k0_26 = 3,       // AD0.3
        k1_30 = 4,       // AD0.4
        k1_31 = 5,       // AD0.5

        /* These ADC channels are compromised on the SJ-One,
         * hence you do not need to support them
         */
        // k0_23 = 0,   // AD0.0
        // k0_24,       // AD0.1
        // k0_3,        // AD0.6
        // k0_2         // AD0.7
    };

    enum ADC_Channel {
        channel_0 = 0,
        channel_1 = 1,
        channel_2 = 2,
        channel_3 = 3,
        channel_4 = 4,
        channel_5 = 5,
        channel_6 = 6,
        channel_7 = 7,
    };


    // Nothing needs to be done within the default constructor
    LabAdc();

    /**
    * 1) Powers up ADC peripheral
    * 2) Set peripheral clock
    * 2) Enable ADC
    * 3) Select ADC channels
    * 4) Enable burst mode
    */
    void AdcInitBurstMode();

    /**
    * 1) Selects ADC functionality of any of the ADC pins that are ADC capable
    *
    * @param pin is the LabAdc::Pin enumeration of the desired pin.
    *
    * WARNING: For proper operation of the SJOne board, do NOT configure any pins
    *           as ADC except for 0.26, 1.31, 1.30
    */
    void AdcSelectPin(Pin pin);

    /**
    * 1) Returns the voltage reading of the 12bit register of a given ADC channel
    * You have to convert the ADC raw value to the voltage value
    * @param channel is the number (0 through 7) of the desired ADC channel.
    */
    float ReadAdcVoltageByChannel(ADC_Channel channel);
private:
    enum pinsel {
        p0_25 = 18,       // AD0.2 <-- Light Sensor -->
        p0_26 = 20,       // AD0.3
        p1_30 = 28,       // AD0.4
        p1_31 = 30,       // AD0.5
    };

};


#endif /* ADCDRIVER_HPP_ */
