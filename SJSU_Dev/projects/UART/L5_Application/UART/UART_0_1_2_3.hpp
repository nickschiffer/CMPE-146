/*
 * UART_0_1_2_3.hpp
 *
 *  Created on: Mar 9, 2019
 *      Author: nickschiffer (nick@schiffer.us)
 */

#ifndef UART_0_1_2_3_HPP_
#define UART_0_1_2_3_HPP_

#include <LPC17xx.h>
#include <char_dev.hpp>

#define QUEUE_SIZE 100
#define ENQUEUE_TIMEOUT 1
#define DEQUEUE_TIMEOUT portMAX_DELAY


class LabUart// : public CharDev
{
 public:
    enum UART_Device {
        U0,
        U1,
        U2,
        U3
    };

    enum BAUD_Rate {
        b600    = 600,
        b1200   = 1200,
        b2400   = 2400,
        b4800   = 4800,
        b9600   = 9600,
        b14400  = 14400,
        b19200  = 19200,
        b38400  = 38400,
        b56000  = 56000,
        b57600  = 57600,
        b115200 = 115200
    };

    enum Frame_Size {
        f5_bit,
        f6_bit,
        f7_bit,
        f8_bit
    };

    enum Stop_Bit {
        s1_bit,
        s2_bit
    };

    enum Parity_Mode {
        pNone,
        pOdd,
        pEven,
        pForced_1,
        pForced_2
    };

    enum UART_Init_Result{
        Success,
        Invalid_Parity_Config,
        No_Device_Initialized,
        Unspecified_Error
    };

    LabUart(){};
    ~LabUart();

    /*
     * Initializes UARTn interface.
     * @param device is the UART_Device enumeration of the desired device.
     * @param baud_rate is the BAUD_Rate enumeration of the desired BAUD rate.
     * @param rx_interrupt determines whether an interrupt will occur upon RX.
     * @param frame_size is the Frame_Size enumeration of the desired frame size.
     * @param break_control determines whether or not break control will occur.
     * @param parity_enable determines whether parity bits will be generated.
     * @param parity_select is the Parity_Select enumeration for desired parity behavior.
     * @return returns UART_Init_Result::Success if Initialization is successful,
     *     other UART_Init_Result status if Initialization has failed
     */
    int Initialize(UART_Device device = U2, BAUD_Rate baud_rate = b38400,
            bool rx_interrupt_enable = true, Frame_Size frame_size = f8_bit, Stop_Bit stop_bit = s1_bit,
            bool break_control = false, bool parity_enable = false, Parity_Mode parity_mode = pNone);
    /*
     * Transmits character on initialized device
     * @param c char to be transmitted
     * @return returns 1 if successful, 0 if failed
     */
    int Transmit(char c);

    /*
     * Returns next char in RX FIFO
     * If RX interrupt is not enabled, this will block until a character is available
     * @return oldest char in RX FIFO
     */
    char Receive();






 private:
    UART_Device this_device = U2;
    bool initialized = false;
    bool intr_enabled = false;

    static void uart0_rx_intr();
    static void uart1_rx_intr();
    static void uart2_rx_intr();
    static void uart3_rx_intr();

    static QueueHandle_t u0_rx_queue;
    static QueueHandle_t u1_rx_queue;
    static QueueHandle_t u2_rx_queue;
    static QueueHandle_t u3_rx_queue;


};



#endif /* UART_2_3_HPP_ */
