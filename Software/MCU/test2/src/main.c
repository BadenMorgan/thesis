#define STM32F051

#include <stdint.h>
#include "stm32f0xx.h"

// This projects demonstrates PWM on the RG LED
// Red:   PB10, TIM2_CH3
// Green: PB11, TIM2_CH4
// Both run on AF2

void init_timer(void);
void init_leds(void);

void init_adc(void);


void main(void) {
    uint32_t delay_cnt;
  init_timer();
  init_leds();
  init_adc();
  while(1){
          // start conversion
    ADC1->CR |= ADC_CR_ADSTART;
    // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
    while((ADC1->ISR & ADC_ISR_EOC) == 0);
    //for(delay_cnt = 0; delay_cnt < 1000000; ++delay_cnt);
    uint8_t adcval = ADC1->DR;
    GPIOB->ODR = adcval;
    uint8_t percentage = adcval/3;
    TIM2->CCR4 = percentage * 80; // Green = 90%
  }
}

void init_timer(void) {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  GPIOB->MODER |= GPIO_MODER_MODER10_1; // PB10 = AF
  GPIOB->MODER |= GPIO_MODER_MODER11_1; // PB11 = AF
  GPIOB->AFR[1] |= (2 << (4*(10 - 8))); // PB10_AF = AF2 (ie: map to TIM2_CH3)
  GPIOB->AFR[1] |= (2 << (4*(11 - 8))); // PB11_AF = AF2 (ie: map to TIM2_CH4)

  TIM2->ARR = 8000;  // f = 1 KHz
  // specify PWM mode: OCxM bits in CCMRx. We want mode 1
  TIM2->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // PWM Mode 1
  TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // PWM Mode 1
  // set PWM percantages
  TIM2->CCR3 = 20 * 80; // Red = 20%
  TIM2->CCR4 = 90 * 80; // Green = 90%

  // enable the OC channels
  TIM2->CCER |= TIM_CCER_CC3E;
  TIM2->CCER |= TIM_CCER_CC4E;

  TIM2->CR1 |= TIM_CR1_CEN; // counter enable
}

void init_leds(void) {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;   //enable clock for LEDs
  GPIOB->MODER |= GPIO_MODER_MODER0_0; //set PB0 to output
  GPIOB->MODER |= GPIO_MODER_MODER1_0; //set PB1 to output
  GPIOB->MODER |= GPIO_MODER_MODER2_0; //set PB2 to output
  GPIOB->MODER |= GPIO_MODER_MODER3_0; //set PB3 to output
  GPIOB->MODER |= GPIO_MODER_MODER4_0; //set PB4 to output
  GPIOB->MODER |= GPIO_MODER_MODER5_0; //set PB5 to output
  GPIOB->MODER |= GPIO_MODER_MODER6_0; //set PB6 to output
  GPIOB->MODER |= GPIO_MODER_MODER7_0; //set PB7 to output
  GPIOB->MODER |= GPIO_MODER_MODER8_0; //set PB7 to output
  GPIOB->MODER |= GPIO_MODER_MODER9_0; //set PB7 to output
  GPIO_INIT()

}

void init_adc(void) {
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN; //enable clock for ADC
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enable clock for port which ADC samples from
  GPIOA->MODER |= GPIO_MODER_MODER5; //set PA5 to analogue mode
  ADC1->CHSELR |= ADC_CHSELR_CHSEL5; // select channel 5
  ADC1->CFGR1 |= ADC_CFGR1_RES_1;    // resolution to 8 bit
  ADC1->CR |= ADC_CR_ADCAL;         // set ADCAL high and wait for it to go low
  while( (ADC1->CR & ADC_CR_ADCAL) != 0);
  ADC1->CR |= ADC_CR_ADEN;           // set ADEN=1 in the ADC_CR register
  while((ADC1->ISR & ADC_ISR_ADRDY) == 0); //wait until ADRDY==1 in ADC_ISR
}
