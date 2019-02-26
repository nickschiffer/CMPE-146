/*
 * LabSpi.cpp
 *
 *  Created on: Feb 24, 2019
 *      Author: Nick
 */

#include <printf_lib.h>
#include <sys/_stdint.h>
#include <SPI/LabSpi.hpp>


bool LabSpi::initialize(uint8_t data_size_select, FrameModes format, uint8_t divide)
{
    /*
     * Bounds checking for parameters
     */
    if ((data_size_select < 4) || (data_size_select > 16))
        return false;
    size = data_size_select;

    if ((divide & 1) || (divide < 2) || (divide > 254))
        return false;

    /*
     * Power on SSP1
     */
    LPC_SC->PCONP |= (1 << pconp_ssp1);
    /*
     * Set port 0.8 [17:16] = 10 for MISO 1
     */
    LPC_PINCON->PINSEL0 |=  (1 << 17);
    LPC_PINCON->PINSEL0 &= ~(1 << 16);
    /*
     *Set port 0.9 [19:18] = 10 for MOSI 1
     */
    LPC_PINCON->PINSEL0 |=  (1 << 19);
    LPC_PINCON->PINSEL0 &= ~(1 << 18);
    /*
     * Set port 0.7 to SCK1 = 10
     */
    LPC_PINCON->PINSEL0 |=  (1 << 15);
    LPC_PINCON->PINSEL0 &= ~(1 << 14);
    /*
     * Set Frame Mode and data size
     */
    LPC_SSP1->CR0 = ((data_size_select - 1) | (format << 4));
    /*
     * Set Clock Divisor
     */
    LPC_SSP1->CPSR |= divide;
    /*
     * SSP Enable
     */
    LPC_SSP1->CR1 = (1 << 1);

    return true;

}


uint8_t LabSpi::transfer(uint8_t send)
{
    //Don't continue unless Initialization has occurred
    if (size == 0)
        return 0xFF;

    spi1_lock();
        while (!(LPC_SSP1->SR & (1 << 1))); //wait for TX FIFO to not be full
        LPC_SSP1->DR = send;
        while((LPC_SSP1->SR & (1 << 4)));   //wait for controller to be idle
        uint8_t result = (uint8_t)(LPC_SSP1->DR);
    spi1_unlock();

    return result;

}


LabSpi::LabSpi()
{
    size = 0;
}


LabSpi::~LabSpi()
{
}




