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

    int l_score = 0;
    int r_score = 0;
    int last_bounce = 0;

#ifndef DEBUG_MODE
    calibrate_adc();
#else
    set_threshold(400);
#endif

    setup_tim7();
    setup_tim6();

    for( ;; ) {
#ifdef DEBUG_MODE
        nano_wait(100000000);
        uint8_t sent_packet = send_packet();
        GPIOC->ODR &= ~GPIO_ODR_6;
        GPIOC->ODR &= ~GPIO_ODR_9;

        sent_packet &= (BOUNCE_LEFT | BOUNCE_RIGHT);
        sent_packet >>= BOUNCEn;

        if(sent_packet != 0)
        {
            if(last_bounce == sent_packet) {
                if(last_bounce == 1) {
                    l_score += 1;
                    GPIOC->ODR |= GPIO_ODR_7;
                } else {
                    r_score += 1;
                    GPIOC->ODR |= GPIO_ODR_8;
                }
                last_bounce = 0;
            } else {
                last_bounce = sent_packet;
                GPIOC->ODR &= ~GPIO_ODR_7;
                GPIOC->ODR &= ~GPIO_ODR_8;
            }


        }
#endif
    }

    return 0;
}