#include "keypad.h"
#include "uart.h"

char keypad_row;

//===========================================================================
// powerup_keypad()
// Configure the pins for matrix keypad and enable EXTI interrupts for them.
//===========================================================================
void powerup_keypad()
{
    // Enable clock to GPIOB
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Enable clock to SYSCFG : EXTI
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;

    // Configure PB0-3 as output : keypad lines
    GPIOB->MODER &= ~0xff;
    GPIOB->MODER |= 0x55;

    // Configure PB4-7 as input : keypad lines
    GPIOB->MODER &= ~0xff00;

    // Configure pull-down resistor for PB4-7
    GPIOB->PUPDR &= ~0xff00;
    GPIOB->PUPDR |= 0xaa00;

    // Enable rising trigger for EXTI PB4-7
    EXTI->RTSR |= 0xf0;

    // Unmask EXTI interrupts on PB4-7
    EXTI->IMR |= 0xf0;

    // Enable EXTI interrupts on PB4-7
    SYSCFG->EXTICR[1] |= 0x1111;

    // Enable interrupt for EXTI4_15
    NVIC->ISER[0] |= (1 << EXTI4_15_IRQn);
}


//===========================================================================
// set_row()
// Set the row active on the keypad matrix.
//===========================================================================
void set_row()
{
    char dec;
    switch(keypad_row) {
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

    // TODO: use BSRR?

    GPIOB->ODR &= ~0xf;
    GPIOB->ODR |= dec;

}

//===========================================================================
// Timer 7 ISR()
// Set the next keypad row high.
//===========================================================================
void TIM7_IRQHandler()
{
    // Acknowledge interrupt
    TIM7->SR &= ~TIM_SR_UIF;

    // Increment keypad_row to set
    keypad_row += 1;
    keypad_row &= 3;
    set_row();
}


//===========================================================================
// setup_tim7()
// Configure timer 7.
//===========================================================================
void setup_tim7()
{
    // Enable clock to TIM7
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


void EXTI4_15_IRQHandler(void)
{
    int keypad_col = EXTI->PR >> 4;
    int pin = keypad_row << 2;

    char key = 0;

    switch(keypad_col)
    {
        case 0x8:
            pin += 3;
            break;
        case 0x4:
            pin += 2;
            break;
        case 0x2:
            pin += 1;
            break;
        case 0x1:
            pin += 0;
            break;
    }

    switch(pin) {
        case 15:
            key = '1';
            break;
        case 14:
            key = '2';
            break;
        case 13:
            key = '3';
            break;
        case 12:
            key = 'A';
            break;
        case 11:
            key = '4';
            break;
        case 10:
            key = '5';
            break;
        case 9:
            key = '6';
            break;
        case 8:
            key = 'B';
            break;
        case 7:
            key = '7';
            break;
        case 6:
            key = '8';
            break;
        case 5:
            key = '9';
            break;
        case 4:
            key = 'C';
            break;
        case 3:
            key = '*';
            break;
        case 2:
            key = '0';
            break;
        case 1:
            key = '#';
            break;
        case 0:
            key = 'D';
            break;
        default:
            key = 0;
            break;
        }

    if(key != 0)
        puts(&key);

    // Clear interrupt flag
    EXTI->PR |= 0xf0;
}
