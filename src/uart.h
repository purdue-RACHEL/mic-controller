/*
 * uart.h
 *
 *  Created on: Aug 28, 2022
 *      Author: micah
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f0xx.h"

enum
{
    REQUEST_DATA = 1,
    RESTART_MICRO,
} Packet;

enum
{
    KEYn = 0,
    KEY_PRESSn = 4,
    BOUNCEn = 5,
    ERRORn = 7,
} Packet_Offsets;

enum
{
    NONE,                       // 0x00

    KEY_PRESS = 0x10,           // 0x10
    ZERO = NONE + KEY_PRESS,    // 0x10
    ONE,                        // 0x11
    TWO,                        // 0x12
    THREE,                      // 0x13
    FOUR,                       // 0x14
    FIVE,                       // 0x15
    SIX,                        // 0x16
    SEVEN,                      // 0x17
    EIGHT,                      // 0x18
    NINE,                       // 0x19
    A,                          // 0x1A
    B,                          // 0x1B
    C,                          // 0x1C
    D,                          // 0x1D
    POUND,                      // 0x1E
    ASTERISK,                   // 0x1F
    KEY_CHAR = ASTERISK,        // 0x1F

    BOUNCE_RIGHT = 0x20,        // 0x20

    /*
     *  values between 0x20 and 0x3F indicate a right bounce,
     *  with any added key presses
     */

    BOUNCE_LEFT = 0x40,         // 0x40

    /*
     *  values between 0x40 and 0x5F indicate a left bounce,
     *  with any added key presses
     */

    /*
     *  values between 0x60 and 0x7F indicate a left and right bounce bounce,
     *  signaling that an error has occurred. as such, the error flag should
     *  high, and these values should not ever be possible.
     */

    ERROR_FLAG = 0x80,          // 0x80

    /*
     * values between 0x80 and 0xff indicate that an error has occurred,
     * and the bottom 4 bits share more information about the error.
     *
     * errors:
     *      0b00000000 - system rebooted
     *      +0b111xxxxx - multiple bounce error
     *      *0b1001xxxx - multiple press error - stores latest press
     *
     *
     *      + indicate we need to increase the poll rate on the laptop
     *      * indicate we (likely) have received a paddle-table hit
     */

} Packet_Status;

extern uint8_t packet;

void setup_uart(void);

int send_packet(void);

void USART3_4_5_6_7_8_IRQHandler(void);

#endif /* UART_H_ */
