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

void setup_tim6(void);

int read_adc(void);

void set_threshold(int x);

#ifdef ADC_CALIBRATE
void calibrate_adc(void);
#endif


#endif /* ADC_H_ */
