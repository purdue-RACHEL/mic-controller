#include "adc.h"
#include "configs.h"
#include "uart.h"

#define NUM_CHANNELS 4
#define NUM_SAMPLES 1000
#define COUNTDOWN 40

uint8_t packet;

uint16_t adc_in[NUM_CHANNELS][NUM_SAMPLES] = {0};
uint32_t adc_index = 0;
uint8_t channel = 0;

uint32_t threshold = 0;

int32_t sum = 0;

int32_t counter = -1;

// TIM6 ISR - 20 kHz
// Reads next ADC channel
void TIM6_DAC_IRQHandler(void)
{
    // Acknowledge interrupt
    TIM6->SR &= ~TIM_SR_UIF;

    // Read ADC conversion
    start_adc_channel(channel);
    adc_in[channel][adc_index] = read_adc();

    if(counter > -1) {
        if(channel == NUM_CHANNELS - 1) {
            adc_index += 1;
            counter += 1;

            if(counter == COUNTDOWN - 1) {
                counter = -1;
                adc_index = 0;
                sum = 0;

                for(int i = 0; i < NUM_CHANNELS; i+=2)
                    for(int j = 0; j < COUNTDOWN; j++) {
                        sum += adc_in[i][j];
                        sum -= adc_in[i+1][j];
                    }

                if(!(packet & (BOUNCE_RED | BOUNCE_BLUE))) {
                    if(sum >= 0) {
                        packet |= BOUNCE_RED;
#ifdef DEBUG_MODE
                        GPIOC->ODR |= GPIO_ODR_6;
#endif
                    } else {
                        packet |= BOUNCE_BLUE;
#ifdef DEBUG_MODE
                        GPIOC->ODR |= GPIO_ODR_9;
#endif
                    }
                } else {
//                    if(sum >= 0)
//                        packet |= BOUNCE_RED;
//                    else
//                        packet |= BOUNCE_BLUE;
//                    packet |= ERROR_FLAG;
                }
            }
        }
    } else {
        if(adc_in[channel][adc_index] > threshold) {
            counter = 0;
            adc_index += 1;
        }
    }

    // Ping-Pong (HA!) buffering
    if(++channel == NUM_CHANNELS) {
        channel = 0;
    }
}

// Setup TIM6
// 20 kHz
void setup_tim6(void)
{
    // Enable RCC to TIM6
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    // Set TIM6 to update 20000 times per second
    TIM6->PSC = 240-1;
    TIM6->ARR = 10-1;

    // Set UIE bit to generate interrupt
    TIM6->DIER |= TIM_DIER_UIE;

    // Enable Timer
    TIM6->CR1 |= TIM_CR1_CEN;

    // Enable interrupt in NVIC
    NVIC->ISER[0] |= (1 << TIM6_DAC_IRQn);
}


// Setup ADC
// Channels 0-1
void setup_adc(void)
{
    // Enable Clock to GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set PA0-3 to analog operation
    // ADC Channels 0-3
    GPIOA->MODER |= GPIO_MODER_MODER0;
    GPIOA->MODER |= GPIO_MODER_MODER1;
    GPIOA->MODER |= GPIO_MODER_MODER2;
    GPIOA->MODER |= GPIO_MODER_MODER3;

#ifdef DEBUG_MODE
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER |= GPIO_MODER_MODER6_0;
    GPIOC->MODER |= GPIO_MODER_MODER7_0;
    GPIOC->MODER |= GPIO_MODER_MODER8_0;
    GPIOC->MODER |= GPIO_MODER_MODER9_0;
#endif


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


// Start ADC Channel n
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


// Set a defined threshold value
void set_threshold(int x)
{
    threshold = x;
}


// Read current ADC conversion
int read_adc(void)
{
    // Wait for EOC bit
    while(!(ADC1->ISR & ADC_ISR_EOC));

    // Return value read from ADC->DR
    return ADC1->DR;
}

// TODO: ignore this until we need trilateration
#ifndef DEBUG_MODE
void calibrate_adc(void)
{
    int mins[8];
    int maxs[8];


    // run 8 tests to pick. for now, use debug to stop
    // todo: read all channels during test
    // todo: "wait for bounce" / use timer

    for(int i = 0; i < 8; i++)
    {
        channel = 0;
        adc_index = 0;

        while(1)
        {

            start_adc_channel(channel);
            adc_in[channel][adc_index] = read_adc();

            nano_wait(1000000);

            if(++adc_index == NUM_SAMPLES)
            {
                mins[i] = adc_in[0][0];
                maxs[i] = adc_in[0][0];

                for(int j = 1; j < NUM_SAMPLES; j++)
                {
                    if(mins[i] > adc_in[0][j])
                        mins[i] = adc_in[0][j];

                    if(maxs[i] < adc_in[0][j])
                        maxs[i] = adc_in[0][j];
                }

                // 0: CONTROL - no drop
                // 1: direct
                // 2: far corner
                // 3: CONTROL
                // 4: side corner
                // 5: side corner
                // 6: CONTROL
                // 7: direct
                // 8: center
                adc_index = 0;
                break;
            }
        }

    }

    // the MINIMUM scanning point
    int zero_point = (maxs[0] + maxs[3] + maxs[6]) / 3;

    // TODO: make robust - ensure this is the largest max val
    // the max detected ball bounce
    // TODO? we can use this as an "upper limit" of sorts? DOWN THE ROAD
    // TODO? have a different threshold per mic?
    int max_point = maxs[1];

    // the (high side) expected bounce noise
    // keep in mind, hitting the ball w/ the paddle will increase the velocity --> increase the signal
    int avg_max_point = (maxs[1] + maxs[2] + maxs[4] + maxs[5] + maxs[7] + maxs[8]) / 6;

    // the safe max point - lower signal
    int safe_max_point = (maxs[2] + maxs[4] + maxs[5]) / 3;

    // "safe" threshold
    threshold = (safe_max_point >> 1) + (safe_max_point >> 2);
}
#endif
