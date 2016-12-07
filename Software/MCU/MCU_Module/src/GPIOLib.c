#include "GPIOLib.h"


/*
setup of the GPIO for on led, status led, vibration motor,
irled 1 and 2, mode selection, re and de, ms 1,2 & 3,
step direction and enable
*/

//set up GPIO as input/output
void GPIOInit(){
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    //MCUON Output
    GPIO_InitStructure.GPIO_Pin = MCUON;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB,MCUON,1);
    //MCUStatus Output
    GPIO_InitStructure.GPIO_Pin = MCUSYS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB,MCUSYS,1);
    //IRLED1
    GPIO_InitStructure.GPIO_Pin = IRLED1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB,IRLED1,0);
    //IRLED2
    GPIO_InitStructure.GPIO_Pin = IRLED2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB,IRLED2,0);
    //MODE
    GPIO_InitStructure.GPIO_Pin = MODE;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //VBRMTR
    GPIO_InitStructure.GPIO_Pin = VBRMTR;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB,VBRMTR,0);
    //DE
    GPIO_InitStructure.GPIO_Pin = DE;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA,DE,0);
    //RE
    GPIO_InitStructure.GPIO_Pin = RE;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA,RE,0);
}

//ADRESS setup
uint8_t InitAdress(){
    GPIO_InitTypeDef GPIO_InitStructure;
    //bit 0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //bit 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //bit 2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //bit 3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //bit 4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //bit 5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //bit 6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //bit 7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    uint16_t pinread = GPIO_ReadInputData(GPIOA);
    uint64_t NewAddress = pinread & 0xFF;
    return NewAddress^0xFF;
}

//initialize 3 adc's for vsense1, vsense2 and alignment
void ADCsInit(){
    RCC->APB2ENR |= RCC_APB2ENR_ADCEN; //enable clock for ADC
    //clock already enabled
    GPIOA->MODER |= GPIO_MODER_MODER0; //set PA0 to analogue mode
    GPIOB->MODER |= GPIO_MODER_MODER0; //set PB0 to analogue mode
    GPIOB->MODER |= GPIO_MODER_MODER1; //set PB1 to analogue mode

    ADC1->CR |= ADC_CR_ADCAL;         // set ADCAL high and wait for it to go low
    while( (ADC1->CR & ADC_CR_ADCAL) != 0);
    ADC1->CR |= ADC_CR_ADEN;           // set ADEN=1 in the ADC_CR register
    while((ADC1->ISR & ADC_ISR_ADRDY) == 0); //wait until ADRDY==1 in ADC_ISR

    ADC1->CHSELR |= ADC_CHSELR_CHSEL9; // select channel 8
    ADC1->CFGR1 |= ADC_CFGR1_RES_1;    // resolution to 8 bit
}

//fetch requested ADC value
uint8_t GetADCVal(uint8_t Channel){
    if(Channel == channel0){
        ADC1->CHSELR = ADC_CHSELR_CHSEL0; // select channel 0
    }else if(Channel == channel8){
        ADC1->CHSELR = ADC_CHSELR_CHSEL8; // select channel 8
    }else if(Channel == channel9){
        ADC1->CHSELR = ADC_CHSELR_CHSEL9; // select channel 9
    }

    ADC1->CR |= ADC_CR_ADSTART;
    // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
    while((ADC1->ISR & ADC_ISR_EOC) == 0);
    return ADC1->DR;
}
