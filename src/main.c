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

    int red_score = 0;
    int blue_score = 0;
    int last_bounce = 0;

    set_threshold(2000);

    setup_tim7();
    setup_tim6();

    // TODO: send reboot packet
    // must wait until polled by laptop

    for( ;; ) {
#ifndef DEBUG_MODE
        nano_wait(10000000);
        uint8_t sent_packet = send_packet();

        sent_packet &= (BOUNCE_RED | BOUNCE_BLUE);

        if(sent_packet != 0) {
            if(last_bounce == sent_packet) {
                if(last_bounce == 1) {
                    red_score += 1;
                } else {
                    blue_score += 1;
                }
                last_bounce = 0;
            } else {
                last_bounce = sent_packet;
            }
        }
#endif
    }

    NVIC_SystemReset();
    return 0;
}
