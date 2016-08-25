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
//#define _EXTRA_
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
uint16_t Stamp[32];     //stamps used for timing 0: idler 1:test 2:taskmanager 3:alignment 4:taskmanager 5: delay 6: wiggle
uint8_t status = 0;     //status to display on the sysled
uint16_t Serialdata[256];    //serial data buffer
uint8_t buffercount = 0;
uint8_t bufferreadcount = 0;
uint16_t period = 1000;     //period used for the idle function
uint8_t modeflag = 0;   //mode flag for selecting adjusting alignment
uint64_t address = 0;   //address byte used to recognize incommoding data
uint8_t testflag = 0;   //test flag used to check if a test is requested
uint16_t releaseval = 250;  //angle *2 at which and IC will be released
uint16_t pickval = 74;  //angle at which an IC will be picked up
uint8_t ICADVVal[10];   //used to average out the IC bucket reading
uint8_t GUIDEADCVal[10];    //used to average out the guide reading
uint8_t ADCCounter = 0;
uint8_t DeliverxMany = 0;   //amount to dispense
uint8_t task = 0;       //task number used for task management
uint8_t jamorempty = 0;


//function definitions
void Ready();
void delay(uint16_t time);
void AlignTaskMng();
void Calibration();
#ifdef _ROLLER_
void TaskManager();
void initTask();
void Test();
void CheckIC();
void Release();
void PickUp();
void CheckIC2();
void CheckLow();
void FinishTask();
void ServoWiggle(uint8_t pos, uint16_t initpos);
void Idler(uint16_t period);
void IntervalHandle(void(*FNCName)(),uint16_t interval,uint8_t stamp);
void ModeSelect();
#endif
#ifdef _DEBUG_
void debuginput();
#endif

int main(void)
{
    GPIOInit();
    address = InitAdress();
    testflag = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
    TIM14Init();
    USART1Init();
    ADCsInit();
#ifdef _ROLLER_
    ServoInit(pickval);
#ifdef _DEBUG_
    USART2Init();
    printBIN(address);
    input();
#endif

    Ready();

    while(1)
    {
        //watchdog
        Idler(period);
#ifdef _DEBUG_
        IntervalHandle(Test,200,1);
#endif
        //IntervalHandle(ADCtest,200,4);
        uint8_t tempmode = modeflag;
        ModeSelect();
        if(modeflag != tempmode){
            ServoSet(pickval);
            task = 0;
        }
#ifdef _DEBUG_
        debuginput();
#endif
        if(modeflag){
            AlignTaskMng();
#ifdef _DEBUG_
            jamorempty = 0;
#endif
        }else if(!modeflag){
            TaskManager();
        }
    }
#endif
}
//ready blink indicating ready to go
void Ready(){
    GPIO_WriteBit(GPIOB,GPIO_Pin_8,0);
    delay(100);
    GPIO_WriteBit(GPIOB,GPIO_Pin_8,1);
    delay(100);
    GPIO_WriteBit(GPIOB,GPIO_Pin_8,0);
    delay(100);
    GPIO_WriteBit(GPIOB,GPIO_Pin_8,1);
    delay(100);
    GPIO_WriteBit(GPIOB,GPIO_Pin_8,0);
    delay(100);
    GPIO_WriteBit(GPIOB,GPIO_Pin_8,1);
    delay(100);
    GPIO_WriteBit(GPIOB,GPIO_Pin_8,0);
    Stamp[0] = TIM_GetCounter(TIM14);
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
    if(sum <= 15000){
        ADC1->CHSELR = ADC_CHSELR_CHSEL0; // select channel 0
        delay(200);
        ADC1->CR |= ADC_CR_ADSTART;
        // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
        while((ADC1->ISR & ADC_ISR_EOC) == 0);
        uint8_t val = ADC1->DR/4;
        if(val > 32){
            val -= 32;
            releaseval = 250 + val;
        }else if(val == 32){
            val = 0;
        }else{
            val = 31 - val;
            releaseval = 250 - val;
        }
        ServoSet(releaseval);
    }else if(sum <= 30000){
        ADC1->CHSELR = ADC_CHSELR_CHSEL0; // select channel 0
        delay(200);
        ADC1->CR |= ADC_CR_ADSTART;
        // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
        while((ADC1->ISR & ADC_ISR_EOC) == 0);
        uint8_t val = ADC1->DR/4;
        if(val > 32){
            val -= 32;
            pickval = 74 + val;
        }else if(val == 32){
            val = 0;
        }else{
            val = 31 - val;
            pickval = 74 - val;
        }
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
            CheckIC();
            break;
        }
        case 3:{
            Release();
            break;
        }
        case 4:{
            ServoWiggle(ReleaseWiggle,releaseval);
            break;
        }
        case 5:{
            Release();
            break;
        }
        case 6:{
            PickUp();
            break;
        }
        case 7:{
            ServoWiggle(PickWiggle,pickval);
            break;
        }
        case 8:{
            CheckIC2();
            break;
        }
        case 9:{
            if(DeliverxMany == 1){
                CheckLow();
            }else{
                task++;
            }
            break;
        }
        default:{
            DeliverxMany--;
            if(DeliverxMany != 0){
                task = 1;
                break;
            }
#ifdef _DEBUG_
            if(!jamorempty){
                printbyte(0x53);
            }
#endif
            FinishTask();
            break;
        }
    }
}
//task 1 turn of receiver and turn on vibration motor
void initTask(){
    if(!jamorempty){
        ServoSet(pickval);  //remove this code for proper operation
        period = 100;
        GPIO_WriteBit(GPIOA,RE,0);
        GPIO_WriteBit(GPIOB,VBRMTR,1);
        GPIO_WriteBit(GPIOB,IRLED1,1);
        GPIO_WriteBit(GPIOB,IRLED2,1);
        task++;
        Stamp[2] = TIM_GetCounter(TIM14);
    }else{
        task = 0xFF;
    }
}

//check for IC in bucket and tube then move to release
void CheckIC(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[2];
    if(sum >= Wait - 150){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 40){
            ICADVVal[ADCCounter] = GetADCVal(channel8);
            GUIDEADCVal[ADCCounter] = GetADCVal(channel9);
            ADCCounter++;
            if(ADCCounter >= 10){
                ADCCounter = 0;
                int i;
                uint16_t ICSum = 0;
                uint16_t GuideSum = 0;
                for(i = 0; i < 10; i++){
                    ICSum += ICADVVal[i];
                    GuideSum += GUIDEADCVal[i];
                }
                ICSum = ICSum/10;
                GuideSum = GuideSum/10;
#ifdef _EXTRA_
                print16bits(ICSum,0x41,3);
                print16bits(GuideSum,0x42,3);
#endif
                if(ICSum < GapThreshold){   //is there IC in bucket
                    ServoSet(releaseval);
                    task++;
                    Stamp[2] = counterval;
                }else if(GuideSum < GapThreshold){  //is the tube empty
                    //if not empty
                    //report jam
                    #ifdef _DEBUG_
                    printbyte(0x4A);
                    #endif
                    jamorempty = 1;
                    task = 0xFF;
                }else{  //tube is empty
                    //report empty
                    #ifdef _DEBUG_
                    printbyte(0x45);
                    #endif
                    jamorempty = 1;
                    task = 0xFF;
                }
            }
            Stamp[4] = counterval;
        }
    }
}

//check IC has dropped hen move to pickup
void Release(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[2];
    if(sum >= Wait - 250){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 40){
            ICADVVal[ADCCounter] = GetADCVal(channel8);
            ADCCounter++;
            if(ADCCounter >= 10){
                ADCCounter = 0;
                int i;
                uint16_t ICSum = 0;
                for(i = 0; i < 10; i++){
                    ICSum += ICADVVal[i];
                }
                ICSum = ICSum/10;
#ifdef _EXTRA_
                print16bits(ICSum,0x43,3);
#endif
                if(ICSum > GapThreshold){   //is there IC in bucket
                    ServoSet(pickval);
                    if(task == 3) task += 3;
                    else task ++;
                    Stamp[2] = counterval;
                }else if(task == 3){
                    Stamp[5] = TIM_GetCounter(TIM14);
                    task++;
                }else{
                    DeliverxMany = 1;
#ifdef _DEBUG_
                    printbyte(0x4A);
#endif
                    jamorempty = 1;
                    task = 0xFF;
                }
            }
            Stamp[4] = counterval;
        }
    }
}

//pickup another IC
void PickUp(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[2];
    if(sum >= Wait - 150){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 40){
            ICADVVal[ADCCounter] = GetADCVal(channel8);
            GUIDEADCVal[ADCCounter] = GetADCVal(channel9);
            ADCCounter++;
            if(ADCCounter >= 10){
                ADCCounter = 0;
                int i;
                uint16_t ICSum = 0;
                uint16_t GuideSum = 0;
                for(i = 0; i < 10; i++){
                    ICSum += ICADVVal[i];
                    GuideSum += GUIDEADCVal[i];
                }
                ICSum = ICSum/10;
                GuideSum = GuideSum/10;
#ifdef _EXTRA_
                print16bits(ICSum,0x41,3);
#endif
                if(ICSum < GapThreshold){   //is there IC in bucket
                    task+=3;
                    Stamp[2] = counterval;
                }else{
                    task++;
                    Stamp[5] = counterval;
                }
            }
            Stamp[4] = counterval;
        }
    }
}

//checks IC but does not end task if empty
void CheckIC2(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[2];
    if(sum >= Wait - 150){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 40){
            ICADVVal[ADCCounter] = GetADCVal(channel8);
            GUIDEADCVal[ADCCounter] = GetADCVal(channel9);
            ADCCounter++;
            if(ADCCounter >= 10){
                ADCCounter = 0;
                int i;
                uint16_t ICSum = 0;
                uint16_t GuideSum = 0;
                for(i = 0; i < 10; i++){
                    ICSum += ICADVVal[i];
                    GuideSum += GUIDEADCVal[i];
                }
                ICSum = ICSum/10;
                GuideSum = GuideSum/10;
#ifdef _EXTRA_
                print16bits(ICSum,0x41,3);
                print16bits(GuideSum,0x42,3);
#endif
                if(ICSum < GapThreshold){   //is there IC in bucket
                    task++;
                    Stamp[2] = counterval;
                }else if(GuideSum < GapThreshold){  //is the tube empty
                    //if not empty
                    //report jam
                    #ifdef _DEBUG_
                    printbyte(0x4A);
                    #endif
                    jamorempty = 1;
                    task = 0xFF;
                }else{  //tube is empty
                    //report empty
                    #ifdef _DEBUG_
                    printbyte(0x45);
                    #endif
                    jamorempty = 1;
                    task = 0xFF;
                }
            }
            Stamp[4] = counterval;
        }
    }
}

//check if tube is low
void CheckLow(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[2];
    if(sum >= Wait - 150){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 40){
            GUIDEADCVal[ADCCounter] = GetADCVal(channel9);
            ADCCounter++;
            if(ADCCounter >= 10){
                ADCCounter = 0;
                int i;
                uint16_t GuideSum = 0;
                for(i = 0; i < 10; i++){
                    GuideSum += GUIDEADCVal[i];
                }
                GuideSum = GuideSum/10;
#ifdef _EXTRA_
                print16bits(GuideSum,0x42,3);
#endif
                if(GuideSum < GapThreshold){  //is the tube empty
                    task++;
                }else{  //tube is empty
                    //report empty
                    #ifdef _DEBUG_
                    printbyte(0x4C);
                    #endif
                    task++;
                }
            }
            Stamp[4] = counterval;
        }
    }
}
//finish off the task and back to idle
void FinishTask(){
    GPIO_WriteBit(GPIOA,RE,1);
    GPIO_WriteBit(GPIOB,VBRMTR,0);
    GPIO_WriteBit(GPIOB,IRLED1,0);
    GPIO_WriteBit(GPIOB,IRLED2,0);
    task = 0;
    period = 1000;
}

//wiggle the servo by the specified amount
void ServoWiggle(uint8_t pos, uint16_t initpos){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[5];
    uint16_t waitfor = 50;
    if(sum <= 25){
        ServoSet(initpos + pos);
    }else if(sum <= (waitfor +25)){
        ServoSet(initpos - pos);
    }else if(sum <= (waitfor*2 +25)){
        ServoSet(initpos + pos);
    }else if(sum <= (waitfor*3 +25)){
        ServoSet(initpos - pos);
    }else if(sum <= (waitfor*4 +25)){
        ServoSet(initpos + pos);
    }else if(sum <= (waitfor*5 +25)){
        ServoSet(initpos - pos);
    }else{
        Stamp[2] = counterval;
        ServoSet(initpos);
        task++;
    }
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
    if(!GPIO_ReadInputDataBit(GPIOB, MODE) && !modeflag){
        modeflag = 1;
    }else if(GPIO_ReadInputDataBit(GPIOB, MODE) && modeflag){
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
        DeliverxMany = 2;
#ifdef _EXTRA_
        print16bits(DeliverxMany,0x43,3);
#endif
        jamorempty = 0;
        task = 1;
    }
}

#ifdef _DEBUG_
/*
debug input function
*/
void debuginput(){
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)){
        DeliverxMany = 3;
        task = 1;
    }else if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)){
        //task = 0;
    }
}
#endif



