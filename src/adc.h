/*
 * adc.h
 *
 *  Created on: Sep 6, 2022
 *      Author: micah
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32f0xx.h"

void setup_adc(void);

void start_adc_channel(int n);

int read_adc(void);

#endif /* ADC_H_ */
