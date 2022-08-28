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

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}


int main()
{
    setup_usart5();

    enable_tty_interrupt();

//    for( ;; )
//        {
//        puts("H");
//        nano_wait(1000000000);
//        }

    enable_ports();
    setup_tim7();
    for( ;; ) {
        char key = getkey();
        if(key != 0)
            printf(key);
    }

    return 0;
}
