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

void setup_uart(void);

int send_packet(void);

void USART3_4_5_6_7_8_IRQHandler(void);

#endif /* UART_H_ */
