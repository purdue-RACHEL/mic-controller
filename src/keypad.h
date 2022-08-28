/*
 * keypad.h
 *
 *  Created on: Aug 28, 2022
 *      Author: micah
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "stm32f0xx.h"

void enable_ports();

void set_row();

int get_cols();

void insert_queue(int n);

void update_hist(int cols);

void TIM7_IRQHandler();

void setup_tim7();

int getkey();

#endif /* KEYPAD_H_ */
