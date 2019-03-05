/*
 * pwmDriver.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: Nick
 */

#include <PWM/pwmDriver.hpp>

uint64_t LabPwm::pr   = 0;
uint64_t LabPwm::mr0  = 0;
uint64_t LabPwm::pclk_rate = 0;
LabPwm::LabPwm(){
    pr = 0;
    mr0 = 0;
    pclk_rate = sys_get_cpu_clock();
}

void LabPwm::PwmSelectAllPins()
{
   //Select pins 2.0 - 2.5 as PWM: 01
   LPC_PINCON->PINSEL4 |= (1 << 0);
   LPC_PINCON->PINSEL4 |= (1 << 2);
   LPC_PINCON->PINSEL4 |= (1 << 4);
   LPC_PINCON->PINSEL4 |= (1 << 6);
   LPC_PINCON->PINSEL4 |= (1 << 8);
   LPC_PINCON->PINSEL4 |= (1 << 10);

   LPC_PINCON->PINSEL4 &= ~(1 << 1);
   LPC_PINCON->PINSEL4 &= ~(1 << 3);
   LPC_PINCON->PINSEL4 &= ~(1 << 5);
   LPC_PINCON->PINSEL4 &= ~(1 << 7);
   LPC_PINCON->PINSEL4 &= ~(1 << 9);
   LPC_PINCON->PINSEL4 &= ~(1 << 11);

   LPC_PINCON->PINMODE4 |= (0xFFF);


   /*
    * Initialize PWM Channels
    */
   LPC_PWM1->PCR &= ~(1 << 2);
   LPC_PWM1->PCR &= ~(1 << 3);
   LPC_PWM1->PCR &= ~(1 << 4);
   LPC_PWM1->PCR &= ~(1 << 5);
   LPC_PWM1->PCR &= ~(1 << 6);

   /*
    * Enable PWM Output on all channels
    */
   LPC_PWM1->PCR |= (1 << 9);
   LPC_PWM1->PCR |= (1 << 10);
   LPC_PWM1->PCR |= (1 << 11);
   LPC_PWM1->PCR |= (1 << 12);
   LPC_PWM1->PCR |= (1 << 13);
   LPC_PWM1->PCR |= (1 << 14);
}

void LabPwm::PwmSelectPin(PWM_Pin pwm_pin_arg)
{
    /*
     * Select pin as PWM
     */
    LPC_PINCON->PINSEL4 |= (1 << (2*pwm_pin_arg));
    /*
     * Set Single Edge Controlled Mode for requested pin
     */
    if (pwm_pin_arg > 0)
        LPC_PWM1->PCR &= ~(1 << (pwm_pin_arg + 1));
    /*
     * Enable PWM output on Requested pin
     */
    LPC_PWM1->PCR |= (1 << (pwm_pin_arg + 9));
}

void LabPwm::PwmInitSingleEdgeMode(uint32_t frequency_Hz)
{
    /*
     * Enable PWM peripheral power and clock
     */
    LPC_SC->PCONP    |=  (1 << pconp_pwm1);
    LPC_SC->PCLKSEL0 |=  (1 << (2*pclk_pwm1));

    /*
     * PR: Prescaler Register Controls Count Rate
     * Want 1Khz default
     * PCLK = 48MHz/4 => 12Mhz
     * 12MHz / (PC + 1) = 1Khz => PC = 11999
     */
    pr = (uint64_t)((((uint32_t)pclk_rate / frequency_Hz)/RESOLUTION) - 1);
    mr0 = RESOLUTION;
    LPC_PWM1->MR0 = (uint32_t)mr0;
    LPC_PWM1->PR  = (uint32_t)pr;
    /*
     * Set to single edge
     */
    LPC_PWM1->PCR &= ~(0x1F << 2);
    /*
     * Set all MR (match counters) to 0 for 0% duty cycle
     */
    LPC_PWM1->MR0 = (uint32_t)mr0;
    LPC_PWM1->MR1 = 0;
    LPC_PWM1->MR2 = 0;
    LPC_PWM1->MR3 = 0;
    LPC_PWM1->MR4 = 0;
    LPC_PWM1->MR5 = 0;
    LPC_PWM1->MR6 = 0;
    /*
     * Reset when TC reaches MR0
     */
    LPC_PWM1->MCR |= (1 << 1);
    /*
     * PWMLER
     */
    LPC_PWM1->LER |= (1 << 0);
    LPC_PWM1->LER |= (1 << 1);
    LPC_PWM1->LER |= (1 << 2);
    LPC_PWM1->LER |= (1 << 3);
    LPC_PWM1->LER |= (1 << 4);
    LPC_PWM1->LER |= (1 << 5);
    LPC_PWM1->LER |= (1 << 6);





    /*
     * Enable Counter
     */

    LPC_PWM1->TCR |= 1;
    LPC_PWM1->TCR |=  (1 << 1);
    LPC_PWM1->TCR &= ~(1 << 1);
    LPC_PWM1->TCR |= (1 << 3);


}

void LabPwm::SetDutyCycle(PWM_Pin pwm_pin_arg, float duty_cycle_percentage)
{
    if ((duty_cycle_percentage < 0) || (duty_cycle_percentage > 1))
        return;
    uint32_t mr = (uint32_t)(duty_cycle_percentage * (float)mr0);

    switch(pwm_pin_arg){
        case k2_0:
            LPC_PWM1->MR1 = mr;
            LPC_PWM1->LER |= (1 << 1);
            break;
        case k2_1:
            LPC_PWM1->MR2 = mr;
            LPC_PWM1->LER |= (1 << 2);
            break;
        case k2_2:
            LPC_PWM1->MR3 = mr;
            LPC_PWM1->LER |= (1 << 3);
            break;
        case k2_3:
            LPC_PWM1->MR4 = mr;
            LPC_PWM1->LER |= (1 << 4);
            break;
        case k2_4:
            LPC_PWM1->MR5 = mr;
            LPC_PWM1->LER |= (1 << 5);
            break;
        case k2_5:
            LPC_PWM1->MR6 = mr;
            LPC_PWM1->LER |= (1 << 6);
            break;
        default:
            return;
    }
}

void LabPwm::SetFrequency(uint32_t frequency_Hz)
{
    if (frequency_Hz <= 0)
        return;
    pr = (uint64_t)((((uint32_t)pclk_rate / frequency_Hz)/RESOLUTION) - 1);
    mr0 = RESOLUTION;
    LPC_PWM1->PR  = (uint32_t)pr;
    LPC_PWM1->MR0 = (uint32_t)mr0;
    LPC_PWM1->LER |= (1 << 0);

}
