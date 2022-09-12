#include "adc.h"

void setup_adc(void)
{
    // Enable Clock to GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set PA0 to analog operation - ADC Channel 0
    GPIOA->MODER |= GPIO_MODER_MODER0;

    // Enable clock to ADC
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // Turn on HSI14
    RCC->CR2 |= RCC_CR2_HSI14ON;

    // Wait for HSI14 to be ready
    while(!(RCC->CR2 & RCC_CR2_HSI14RDY));

    // Enable ADC
    ADC1->CR |= ADC_CR_ADEN;

    // Wait for ADC to be ready
    while(!(ADC1->ISR & ADC_ISR_ADRDY));

}

void start_adc_channel(int n)
{
    // Select ADC channel
    ADC1->CHSELR = 0;
    ADC1->CHSELR = 1 << n;

    // Wait for ADRDY bit to be set
    while(!(ADC1->ISR & ADC_ISR_ADRDY));

    // Set ADSTART bit
    ADC1->CR |= ADC_CR_ADSTART;

}

//============================================================================
// read_adc()    (Autotest #3)
// Wait for A-to-D conversion to complete, and return the result.
// Parameters: none
// Return value: converted result
//============================================================================
int read_adc(void)
{
    // Wait for EOC bit
    while(!(ADC1->ISR & ADC_ISR_EOC));

    // Return value read from ADC->DR
    return ADC1->DR;
}
