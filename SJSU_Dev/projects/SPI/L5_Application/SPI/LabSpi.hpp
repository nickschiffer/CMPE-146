/*
 * LabSpi.hpp
 *
 *  Created on: Feb 23, 2019
 *      Author: Nick
 */

#ifndef LABSPI_HPP_
#define LABSPI_HPP_

#include "stdint.h"
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "tasks.hpp"
#include "GPIO/GPIO_0_1_2.hpp"
#include "spi_sem.h"

#define SSP1_CS_PORT 0
#define SSP1_CS_PIN  6



class LabSpi
{
 public:
    enum FrameModes
    {
        tSPI,
        tTI,
        tMicrowire
    };

    /*
     * Status from SSP SR register
     */
    typedef union {
        uint8_t byte;
        struct{
            bool TX_FIFO_EMPTY    : 1;
            bool TRANSMIT_NOT_FULL: 1;
            bool RX_FIFO_NOT_EMPTY: 1;
            bool RX_FIFO_FULL     : 1;
            bool BUSY             : 1;
        }__attribute__((packed));
    }SSP_SR_t;

    /*
     * Status from SPI Flash device
     * see p. 29 in adesto AT45DB161E manual
     */
    typedef union {
        uint16_t bytes;
        struct{
            uint8_t byte1;
            struct{
                bool    PAGE_SIZE  : 1;
                bool    PROTECT    : 1;
                uint8_t DENSITY    : 4;
                bool    COMP       : 1;
                bool    READY      : 1;
            }__attribute__((packed));
            uint8_t byte2;
            struct{
                bool ES     : 1;
                bool PS1    : 1;
                bool PS2    : 1;
                bool SLE    : 1;
                bool        : 1;
                bool EPE    : 1;
                bool        : 1;
                bool READY2 : 1;
            }__attribute__((packed));
        }__attribute__((packed));
    }SPI_SR_t;

//    typedef union {
//            uint8_t byte1;
//            struct{
//                bool    READY     : 1;
//                bool    COMP      : 1;
//                uint8_t DENSITY   : 4;
//                bool    PROTECT   : 1;
//                bool    PAGE_SIZE : 1;
//            }__attribute__((packed));
//            uint8_t byte2;
//            struct{
//                bool READY2 : 1;
//                bool        : 1;
//                bool EPE    : 1;
//                bool        : 1;
//                bool SLE    : 1;
//                bool PS2    : 1;
//                bool PS1    : 1;
//                bool ES     : 1;
//            }__attribute__((packed));
//        }SPI_SR_t;

    /**
     * 1) Powers on SPPn peripheral
     * 2) Set peripheral clock
     * 3) Sets pins for specified peripheral to MOSI, MISO, and SCK
     *
     * @param data_size_select transfer size data width; To optimize the code, look for a pattern in the datasheet
     * @param format is the code format for which synchronous serial protocol you want to use.
     * @param divide is the how much to divide the clock for SSP; take care of error cases such as the value of 0, 1, and odd numbers
     *
     * @return true if initialization was successful
     */
    bool initialize(uint8_t data_size_select, FrameModes format, uint8_t divide);

    /**
     * Transfers a byte via SSP to an external device using the SSP data register.
     * This region must be protected by a mutex static to this class.
     *
     * @return received byte from external device via SSP data register.
     */
    uint8_t transfer(uint8_t send);

    inline bool isBusy(){return ((LPC_SSP1->SR & (1 << 4)) ? true : false);}

    LabSpi();
    ~LabSpi();

 private:
    uint8_t size;

};




#endif /* LABSPI_HPP_ */
