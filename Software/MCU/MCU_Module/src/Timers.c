#include "Timers.h"

/*
setup timer 14 for unit time counting ticking at 0.5ms
*/

void TIM14Init(){

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    //default value
    TIM_TimeBaseStructure.TIM_Prescaler = 48000;
    //if timing bug adjust for calibration
    //TIM_TimeBaseStructure.TIM_Prescaler = 4800*2;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
    /* Configure CH1 in PWM1 Mode */
    //TIM_ARRPreloadConfig(TIM1, ENABLE);
    /* Enable TIM3 counter */
    TIM_Cmd(TIM14, ENABLE);
}
