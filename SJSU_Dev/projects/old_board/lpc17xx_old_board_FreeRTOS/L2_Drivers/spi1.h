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
 * @file    spi1.h
 * @ingroup Drivers
 */
#ifndef SPI1_H_
#define SPI1_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "LPC17xx.h"


/**
 * Initializes SPI 1
 * Configures CLK, MISO, MOSI pins with a slow SCK speed
 */
void spi1_Init();

/**
 * Sets SPI Clock speed
 * @param maxClockMhz   The maximum speed of this SPI in Megahertz
 * @note The speed may be set lower to maxClockMhz if it cannot be attained.
 */
void spi1_SetMaxClockMhz(unsigned int maxClockMhz);

/**
 * Exchanges a byte over SPI bus
 * @param out   The byte to send out
 * @returns     The byte received over SPI
 */
inline char spi1_ExchangeByte(char out)
{
    LPC_SSP1->DR = out;
    while(LPC_SSP1->SR & (1 << 4)); // Wait until SSP is busy
    return LPC_SSP1->DR;
}

/**
 * Exchanges 8 bytes over SPI BUS.
 * This is a more efficient function than exchanging a byte 8 times.
 * Outgoing data is sent from pEightBytes, and incoming data is copied onto pEightBytes.
 */
inline void spi1_ExchangeEightBytes(char *pEightBytes)
{
    char *pOut = pEightBytes;
    char *pIn = pEightBytes;

    LPC_SSP1->DR = *pOut++; LPC_SSP1->DR = *pOut++; LPC_SSP1->DR = *pOut++; LPC_SSP1->DR = *pOut++;
    LPC_SSP1->DR = *pOut++; LPC_SSP1->DR = *pOut++; LPC_SSP1->DR = *pOut++; LPC_SSP1->DR = *pOut++;

    while(LPC_SSP1->SR & (1 << 4)); // Wait until SSP is busy

    *pIn++ = LPC_SSP1->DR; *pIn++ = LPC_SSP1->DR; *pIn++ = LPC_SSP1->DR; *pIn++ = LPC_SSP1->DR;
    *pIn++ = LPC_SSP1->DR; *pIn++ = LPC_SSP1->DR; *pIn++ = LPC_SSP1->DR; *pIn++ = LPC_SSP1->DR;
}

/**
 * Transfers data over SPI (SSP#1)
 * @param pBuffer   The read or write buffer
 * @param numBytes  The length of the transfer in bytes
 * @param isWriteOp Non-zero for Write-operation, zero for read operation
 *
 * @note If isWriteOp is true:
 *          - SPI data is sent from pBuffer
 *          - SPI data is copied from SSP DR to dummy buffer and discarded
 *       If isWriteOp is false (read operation) :
 *          - SPI data is copied from SSP DR to pBuffer
 *          - 0xFF is sent out for each byte transfered
 *
 * @return 0 upon success, or non-zero upon failure.
 */
unsigned spiDmaTransferBlock(unsigned char* pBuffer, uint32_t numBytes, char isWriteOp);



#ifdef __cplusplus
}
#endif
#endif /* SPI1_H_ */
