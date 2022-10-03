#include "keypad.h"
#include "uart.h"

char keypad_row;
char hist[16];

uint8_t packet = 0;

// Setup keypad
void setup_keypad()
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


// Set the next keypad row
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

    GPIOB->ODR &= ~0xf;
    GPIOB->ODR |= dec;
}


// TIM7 ISR - 1 kHz
// update history & set next row
void TIM7_IRQHandler()
{
    // Acknowledge interrupt
    TIM7->SR &= ~TIM_SR_UIF;

    // Update key history - debouncing
    for(int i = 0; i <= 15; i++)
        hist[i] <<= 1;

    // Increment keypad_row to set
    keypad_row += 1;
    keypad_row &= 3;

    set_row();
}


// Setup TIM7
// 1 kHz
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


// EXTI 4-7 ISR
// called on button press
void EXTI4_15_IRQHandler(void)
{
    int keypad_col = EXTI->PR >> 4;
    int pin = keypad_row << 2;

    // TODO: check the rest of the unchecked rows?
    // to ensure no missed double press?

    uint8_t key = 0;

    // Get column
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

    hist[pin] |= 1;

    // Ensure not bouncing
    if(hist[pin] != 0x1)
    {
        EXTI->PR |= 0xf0;
        return;
    }

    // Get key
    switch(pin) {
        case 15:
            key = ONE;
            break;
        case 14:
            key = TWO;
            break;
        case 13:
            key = THREE;
            break;
        case 12:
            key = A;
            break;
        case 11:
            key = FOUR;
            break;
        case 10:
            key = FIVE;
            break;
        case 9:
            key = SIX;
            break;
        case 8:
            key = B;
            break;
        case 7:
            key = SEVEN;
            break;
        case 6:
            key = EIGHT;
            break;
        case 5:
            key = NINE;
            break;
        case 4:
            key = C;
            break;
        case 3:
            key = ASTERISK;
            break;
        case 2:
            key = ZERO;
            break;
        case 1:
            key = POUND;
            break;
        case 0:
            key = D;
            break;
        default:
            key = NONE;
            return;
        }

    // A multi-press error has occurred
    if(packet & KEY_PRESS) {
        packet |= ERROR_FLAG;
        packet &= ~KEY_CHAR;
    }

    packet |= key;

    // Clear interrupt flag
    EXTI->PR |= 0xf0;
}
