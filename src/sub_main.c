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

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}


int main()
{
    int z[1000] = {-1};
    int index = 0;

    setup_usart5();

    enable_tty_interrupt();

    setup_adc();

    powerup_keypad();
    setup_tim7();

    for( ;; ) {

        start_adc_channel(0);

//        int s = read_adc();
//        if (s > 1500)
//        {
//            printf(s);
//        }

        z[index] = read_adc();

        if(z[index] > 1800) // or 0
            printf(z[index-1]);

        if(++index == 1000)
        {
            index = 0;

            int max = z[0];
            int maxindex = 0;
            int min = z[0];
            int minindex = 0;

            for(int i = 1; i < 1000; i++)
            {
                if(z[i] > max)
                {
                    max = z[i];
                    maxindex = i;
                }

                if(z[i] < min)
                {
                    min = z[i];
                    minindex = i;
                }

            }
            printf(max);
        }
    }

    return 0;
}
