#include "uart.h"

void setup_usart5(void)
{
    // Enable RCC to GPIOC, GPIOD
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;

    // PC12 -> USART5_TX (AF2)
    // PD2 -> USART5_RX (AF2)
    GPIOC->MODER &= ~GPIO_MODER_MODER12;
    GPIOC->MODER |= GPIO_MODER_MODER12_1;
    GPIOC->AFR[1] &= ~GPIO_AFRH_AFR12;
    GPIOC->AFR[1] |= 0x00020000;


    GPIOD->MODER &= ~GPIO_MODER_MODER2;
    GPIOD->MODER |= GPIO_MODER_MODER2_1;
    GPIOD->AFR[0] &= ~GPIO_AFRL_AFR2;
    GPIOD->AFR[0] |= 0x00000200;

    // Enable RCC to USART5
    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;

    // Configure USART5:
    // Disable USART5
    USART5->CR1 &= ~USART_CR1_UE;

    // Set to 8N1
    USART5->CR1 &= ~USART_CR1_M;
    USART5->CR1 &= ~USART_CR1_PCE;
    USART5->CR2 &= ~USART_CR2_STOP;

    // 16x oversampling
    USART5->CR1 &= ~USART_CR1_OVER8;

    // 115.2 kbaud
    USART5->BRR = 0x1A1;

    // Enable Tx, Rx
    USART5->CR1 |= USART_CR1_TE;
    USART5->CR1 |= USART_CR1_RE;

    // Enable USART5
    USART5->CR1 |= USART_CR1_UE;

    // Wait for TE and RE acknowledgement
    while(!(USART5->ISR & USART_ISR_TEACK));
    while(!(USART5->ISR & USART_ISR_REACK));
}


int putchar(int ch)
{
    while(!(USART5->ISR & USART_ISR_TXE));

    USART5->TDR = ch;

    return ch;
}


int getchar(void)
{
    while(!(USART5->ISR & USART_ISR_RXNE));

    if(USART5->RDR == '\r')
        return '\n';

    return USART5->RDR;
}


int __io_getchar(void)
{
    return getchar();
}


void USART3_4_5_6_7_8_IRQHandler(void)
{
    // Check & Clear ORE flag
    if (USART5->ISR & USART_ISR_ORE)
        USART5->ICR |= USART_ICR_ORECF;

    // Read new character
    char ch = USART5->RDR;

    if( ch != '\n' || ch != '\r' )
        {
        // TODO: reads \n on 2nd pass always
        printf(ch);
        }
}


void enable_tty_interrupt(void)
{
    // Configure USART5 to raise interrupt when RDR not empty
    USART5->CR1 |= USART_CR1_RXNEIE;

    // Enable interrupt in NVIC_ISER
    NVIC->ISER[0] |= (1 << USART3_8_IRQn);
}
