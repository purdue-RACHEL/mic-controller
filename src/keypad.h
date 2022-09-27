/*
 * keypad.h
 *
 *  Created on: Aug 28, 2022
 *      Author: micah
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "stm32f0xx.h"

void setup_keypad();

void set_row();

void TIM7_IRQHandler();

void setup_tim7();

#endif /* KEYPAD_H_ */
