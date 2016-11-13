#include "Servo.h"

//code has been ported from James Gowans Code on PWM control
//setup timer 2 for PWM on channel 4
void ServoInit(uint8_t startval){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    GPIOB->MODER |= GPIO_MODER_MODER11_1; // PB11 = AF
    GPIOB->AFR[1] |= (2 << (4*(11 - 8))); // PB11_AF = AF2 (ie: map to TIM2_CH4)

    TIM2->PSC = 266;
    TIM2->ARR = 3600;  // f = 1 KHz
    // specify PWM mode: OCxM bits in CCMRx. We want mode 1
    TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // PWM Mode 1
    // set PWM percantages
    TIM2->CCR4 = 90 + startval; // Green = 90%

    // enable the OC channels
    TIM2->CCER |= TIM_CCER_CC4E;

    TIM2->CR1 |= TIM_CR1_CEN; // counter enable
}

//sweep function to test the servo
void ServoSweep(){
    TIM2->CCR4 += 1;
    if(TIM2->CCR4 >+ 451){
        TIM2->CCR4 = 90;
    }
}

//set the angle on the servo
void ServoSet(uint16_t angle){
    TIM2->CCR4 = 90 + angle;
}

//set the servo to zero to prevent holding current
void ServoZero(){
    TIM2->CCR4 = 0;
}

