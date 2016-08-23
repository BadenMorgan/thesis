/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 01
   Last changed by:    $Author: Baden Morgan
   Last changed date:  $Date:  24.08.2016
   ID:                 $Id:  MCU_Module

   Resources used as references to assist coding include James Gowans
   Github Repo: https://github.com/jgowans, and the STM peripheral
   library manual


**********************************************************************/
//macros
#define _DEBUG_
#define _ROLLER_

//Libraries
#include "stm32f0xx_conf.h"
#include "SerialMonitor.h"
#include "Timers.h"
#include "GPIOLib.h"
#include "Servo.h"
#include "definitions.h"

#ifdef _DEBUG_
#include "Debug.h"
#endif


//global variables
uint16_t Stamp[32];     //stamps used for timing 0: idler 1:test 2:taskmanager 3:alignment 5: delay
uint8_t status = 1;     //status to display on the sysled
uint16_t Serialdata[64];    //serial data buffer
uint16_t period = 1000;     //period used for the idle function
uint8_t task = 0;       //task number used for task management
uint8_t modeflag = 0;   //mode flag for selecting adjusting alignment
uint64_t address = 0;   //address byte used to recognize incommoding data
uint8_t testflag = 0;   //test flag used to check if a test is requested
uint16_t releaseval = 250;
uint16_t pickval = 74;


//objects definitions

//function definitions
void delay(uint16_t time);
void AlignTaskMng();
void Calibration();
#ifdef _ROLLER_
void TaskManager();
void initTask();
void Test();
void CheckIC();
void FinishTask();
void Idler(uint16_t period);
void IntervalHandle(void(*FNCName)(),uint16_t interval,uint8_t stamp);
void ModeSelect();
void Test();
#endif
#ifdef _DEBUG_
void input();
void debuginput();
void printBIN(uint8_t val);
void print16bits(uint16_t value, uint8_t pre, uint8_t len);
void ADCtest();
#endif

int main(void)
{
    GPIOInit();
    address = InitAdress();
    TIM14Init();
    USART1Init();
    ADCsInit();
#ifdef _ROLLER_
    ServoInit();
    ServoSet(pickval);
#ifdef _DEBUG_
    USART2Init();
    printBIN(address);
    input();
#endif

    while(1)
    {
        //watchdog
        Idler(period);
        IntervalHandle(Test,200,1);
        IntervalHandle(ADCtest,200,4);
        uint8_t tempmode = modeflag;
        ModeSelect();
        if(modeflag != tempmode){
            task = 0;
        }
#ifdef _DEBUG_
        debuginput();
#endif
        if(modeflag){
            AlignTaskMng();
        }else if(!modeflag){
            TaskManager();
        }
    }
#endif
}
//delay function
void delay(uint16_t time){
    Stamp[5] = TIM_GetCounter(TIM14);
    uint16_t sum = TIM_GetCounter(TIM14) - Stamp[5];
    while(sum <= time){
        sum = TIM_GetCounter(TIM14) - Stamp[5];
    }
}
//task manager for alignment calibration
void AlignTaskMng(){
    switch(task){
        case 0:{
            FinishTask();
            period = 500;
            task ++;
            Stamp[3] = TIM_GetCounter(TIM14);
            break;
        }
        case 1:{
            Calibration();
            break;
        }
        default:{
            ServoSet(pickval);
            task = 0;
            break;
        }
    }
}
//calibration function to align servo
void Calibration(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[3];
    if(sum <= 10000){
        ServoSet(releaseval);
    }else if(sum <= 20000){
        ServoSet(pickval);
    }else{
        task++;
    }
}
//function used if roller is being used
#ifdef _ROLLER_
//tasks
//task manager
void TaskManager(){
    switch(task){
        case 0:{
            period = 1000;
            break;
        }
        case 1:{
            initTask();
            break;
        }
        case 2:{
            uint16_t counterval = TIM_GetCounter(TIM14);
            uint16_t sum = counterval - Stamp[2];
            if(sum >= 500){
                CheckIC();
                Stamp[2] = counterval;
            }
            break;
        }
        case 3:{

            break;
        }
        case 4:{

            break;
        }
        case 5:{
            break;
        }
        case 6:{
            break;
        }
        case 7:{
            break;
        }
        case 8:{
            break;
        }
        case 9:{
            break;
        }
        default:{
            FinishTask();
            break;
        }
    }
}
//task 1 turn of receiver and turn on vibration motor
void initTask(){
    period = 100;
    GPIO_WriteBit(GPIOA,RE,0);
    GPIO_WriteBit(GPIOB,VBRMTR,1);
    GPIO_WriteBit(GPIOB,IRLED1,1);
    GPIO_WriteBit(GPIOB,IRLED2,1);
    task++;
    Stamp[2] = TIM_GetCounter(TIM14);
}

void CheckIC(){
    task = 0xFF;
}

void FinishTask(){
    GPIO_WriteBit(GPIOA,RE,1);
    GPIO_WriteBit(GPIOB,VBRMTR,0);
    GPIO_WriteBit(GPIOB,IRLED1,0);
    GPIO_WriteBit(GPIOB,IRLED2,0);
    task = 0;
    period = 1000;
}
#endif
/*
idler function to toggle led for status indication
*/
void Idler(uint16_t period){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[0];
    if(sum >= period){
        status ^= 1;
        GPIO_WriteBit(GPIOB,GPIO_Pin_8,status);
        Stamp[0] = counterval;
    }
}

/*
run a function at set intervals
*/
void IntervalHandle(void(*FNCName)(),uint16_t interval,uint8_t stamp){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[stamp];
    if(sum >= interval){
        FNCName();
        Stamp[stamp] = counterval;
    }
}

/*
mode selection function
*/
void ModeSelect(){
    if(GPIO_ReadInputDataBit(GPIOB, MODE) && !modeflag){
        modeflag = 1;
    }else if(!GPIO_ReadInputDataBit(GPIOB, MODE) && modeflag){
        modeflag = 0;
    }
}

/*
dispensary test
*/
void Test(){
    uint8_t tempbit = testflag;
    testflag = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
    if(tempbit != testflag){
        task = 1;
    }

}

#ifdef _DEBUG_
/*
debug input button
*/
void input(){
    GPIO_InitTypeDef GPIO_InitStructure;
    //MODE
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
debug input function
*/
void debuginput(){
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)){
        task = 1;
    }else if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)){
        //task = 0;
    }
}

/*
print the address obtained by the initialization
*/
void printBIN(uint8_t val){
    int i;
    for( i = 0; i < 8 ; i++){
        if(val & (0b10000000 >> i)){
            USART_SendData(USART2, 0x31);
            /* Loop until the end of transmission */
            while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
        }else{
            USART_SendData(USART2, 0x30);
            /* Loop until the end of transmission */
            while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
        }
    }
    USART_SendData(USART2, 0x0A);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
}

/*
output a value on the serial monitor
*/
void print16bits(uint16_t value, uint8_t pre, uint8_t len){
    USART_SendData(USART2, pre);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
    USART_SendData(USART2, 0x0A);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
    uint8_t split[5];
    split[4] = (value/10000);
    split[3] = (value - (split[4]*10000))/1000;
    split[2] = (value - split[4]*10000 - split[3]*1000)/100;
    split[1] = (value - split[4]*10000 - split[3]*1000 - split[2]*100)/10;
    split[0] = (value - split[4]*10000 - split[3]*1000 - split[2]*100 - split[1]*10);
    int i;
    for(i = len ; i > 0  ; i--){
        USART_SendData(USART2, split[i-1] + 48);
        /* Loop until the end of transmission */
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
    }
    USART_SendData(USART2, 0x0A);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
}

/*
test the adc
*/
void ADCtest(){
    /*ADC1->CHSELR |= ADC_CHSELR_CHSEL0; // select channel 0
    delay(200);
    ADC1->CR |= ADC_CR_ADSTART;
    // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
    while((ADC1->ISR & ADC_ISR_EOC) == 0);
    print16bits(ADC1->DR,0x41,3);*/



    ADC1->CHSELR |= ADC_CHSELR_CHSEL8; // select channel 8
    ADC1->CR |= ADC_CR_ADSTART;
    // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
    while((ADC1->ISR & ADC_ISR_EOC) == 0);
    print16bits(ADC1->DR,0x42,3);

    ADC1->CHSELR |= ADC_CHSELR_CHSEL9; // select channel 9
    ADC1->CR |= ADC_CR_ADSTART;
    // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
    while((ADC1->ISR & ADC_ISR_EOC) == 0);
    print16bits(ADC1->DR,0x43,3);

    ADC1->CHSELR |= ADC_CHSELR_CHSEL9; // select channel 9
    ADC1->CR |= ADC_CR_ADSTART;
    // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
    while((ADC1->ISR & ADC_ISR_EOC) == 0);
    print16bits(ADC1->DR,0x43,3);


}
#endif






