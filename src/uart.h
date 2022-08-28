/*
 * uart.h
 *
 *  Created on: Aug 28, 2022
 *      Author: micah
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f0xx.h"

void setup_usart5(void);

int putchar(int ch);

int getchar(void);

int __io_putchar(int ch);

int __io_getchar(void);

void USART3_4_5_6_7_8_IRQHandler(void);

void enable_tty_interrupt(void);

#endif /* UART_H_ */
