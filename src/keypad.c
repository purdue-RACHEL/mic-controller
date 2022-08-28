#include "keypad.h"

char offset;
char history[16];
char display[8];
char queue[2];
int  qin;
int  qout;

//===========================================================================
// enable_ports()    (Autotest #3)
// Configure the pins for GPIO Ports B and C.
// Parameters: none
//===========================================================================
void enable_ports()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Configure PC0-10 as output
    GPIOC->MODER &= 0xffc00000;
    GPIOC->MODER |= 0x00155555;

    // Configure PB0-PB3 as output
    GPIOB->MODER &= ~0xff;
    GPIOB->MODER |= 0x55;

    // Configure PB4-7 as input
    GPIOB->MODER &= ~0xff00;

    // configure PDR for PB4-PB7
    GPIOB->PUPDR &= ~0xff00;
    GPIOB->PUPDR |= 0xaa00;
}


//===========================================================================
// set_row()    (Autotest #5)
// Set the row active on the keypad matrix.
// Parameters: none
//===========================================================================
void set_row()
{
    char digit = offset &= 0x3;

    char dec;
    switch(digit) {
    case 3:
        dec = 0x8;
        break;
    case 2:
        dec = 0x4;
        break;
    case 1:
        dec = 0x2;
        break;
    default:
        dec = 0x1;
    }

    GPIOB->ODR &= ~0xf;
    GPIOB->ODR |= dec;

}


//===========================================================================
// get_cols()    (Autotest #6)
// Read the column pins of the keypad matrix.
// Parameters: none
// Return value: The 4-bit value read from PC[7:4].
//===========================================================================
int get_cols()
{
    int cols = GPIOB->IDR;
    cols >>= 4;
    cols &= 0xf;
    return(cols);
}


//===========================================================================
// insert_queue()    (Autotest #7)
// Insert the key index number into the two-entry queue.
// Parameters: n: the key index number
//===========================================================================
void insert_queue(int n)
{
    n |= 0x80;
    queue[qin] = n;
    if(qin == 1)
        qin = 0;
    else
        qin = 1;
}


//===========================================================================
// update_hist()    (Autotest #8)
// Check the columns for a row of the keypad and update history values.
// If a history entry is updated to 0x01, insert it into the queue.
// Parameters: none
//===========================================================================
void update_hist(int cols)
{
    int row = offset & 0x3;


    for(int i = 0; i < 4; i++) {
        history[4 * row + i] <<= 1;
        history[4 * row + i] |= (cols >> i) & 1;

        if(cols >> i == 0x1)
            insert_queue(4 * row + i);
    }

}


//===========================================================================
// Timer 7 ISR()    (Autotest #9)
// This is the Timer 7 ISR
// Parameters: none
// (Write the entire subroutine below.)
//===========================================================================
void TIM7_IRQHandler()
{
    // Acknowledge interrupt
    TIM7->SR &= ~TIM_SR_UIF;

    int cols = get_cols();
    update_hist(cols);
    offset += 1;
    offset &= 7;
    set_row();

}


//===========================================================================
// setup_tim7()    (Autotest #10)
// Configure timer 7
// Parameters: none
//===========================================================================
void setup_tim7()
{
    // Enable RCC for TIM7
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    // Set PSC, ARR -> freq of 1kHz
    TIM7->PSC = 4800 - 1;
    TIM7->ARR = 10 - 1;

    // Enable UIE bit in DIER
    TIM7->DIER |= TIM_DIER_UIE;

    // Enable TIM7 interrupt in NVIC
    NVIC->ISER[0] |= (1 << TIM7_IRQn);

    // Enable TIM7 in CR1
    TIM7->CR1 |= TIM_CR1_CEN;
}


//===========================================================================
// getkey()    (Autotest #11)
// Wait for an entry in the queue.  Translate it to ASCII.  Return it.
// Parameters: none
// Return value: The ASCII value of the button pressed.
//===========================================================================
int getkey()
{
    do {
        asm volatile("wfi");
    } while(queue[qout] == 0);

    int hist = queue[qout] & 0x7f;
    queue[qout] = 0;

    if(qout == 0)
        qout = 1;
    else
        qout = 0;

    switch(hist) {
    case 15:
        return '1';
    case 14:
        return '2';
    case 13:
        return '3';
    case 12:
        return 'A';
    case 11:
        return '4';
    case 10:
        return '5';
    case 9:
        return '6';
    case 8:
        return 'B';
    case 7:
        return '7';
    case 6:
        return '8';
    case 5:
        return '9';
    case 4:
        return 'C';
    case 3:
        return '*';
    case 2:
        return '0';
    case 1:
        return '#';
    case 0:
        return 'D';
    }

    return 0; // replace this
}
