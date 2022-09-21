/*
 * ECE 477 - R.A.C.H.E.L.
 *
 * sub_main.c - the main loop for sub controller
 *
 * Last edited by: Micah Morefield
 * Last edit: 8/28/22
 */

#include "stm32f0xx.h"
#include <stdio.h>
#include "uart.h"
#include "keypad.h"
#include "adc.h"
#include "configs.h"

#ifdef DEBUG_MODE
void nano_wait(unsigned int n) {
    asm("mov r0,%0\n"
        "repeat: sub r0,#83\n"
        "bgt repeat\n" : : "r"(n) : "r0", "cc");
}
#endif


int main()
{
    setup_uart();
    setup_adc();
    setup_keypad();

#ifndef DEBUG_MODE
    calibrate_adc();
#else
    set_threshold(400);
#endif

    setup_tim7();
    setup_tim6();

    for( ;; ) {
#ifdef DEBUG_MODE
        nano_wait(10000000);
        send_packet();
        GPIOC->ODR &= ~GPIO_ODR_6;
        GPIOC->ODR &= ~GPIO_ODR_9;
#endif
    }

    return 0;
}
