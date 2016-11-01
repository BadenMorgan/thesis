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
//#define _EXTRA_   //defined extra debugging
#define _DEBUG_     //debugging serial interface
#define _ROLLER_    //roller code so it can be excluded for another variation of delivery mechanism
//#define _LCD_       //LCD code excluded for most delivery mechanisms, only used on first one

//defines which package is being dispensed, important for wait times
//#define _DIP8_
//#define _DIP1416_
#define _DIP20_

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

#ifdef _LCD_
#include "lcd.h"
#endif


//global variables
uint16_t Stamp[10];                     //stamps used for timing 0: idler 1:test 2:taskmanager 3:alignment 4:taskmanager 5: delay 6: wiggle 7:debug coms 8:scrolling 9defaulting lcd 9: timing
uint8_t status = 0;                     //status to display on the sysled
uint8_t Serialdata[256];                //serial data buffer
uint8_t buffercount = 0;                //counts serial data buffer position
uint8_t bufferreadcount = 0;            //counts how many unread bytes in serialdata buffer
uint8_t RXFlag = 2;                     //used to determine if data has stopped coming through
uint16_t period = 1000;                 //period used for the idle function
uint8_t modeflag = 0;                   //mode flag for selecting adjusting alignment
uint64_t address = 0;                   //address byte used to recognize incommoding data
uint8_t testflag = 0;                   //test flag used to check if a test is requested
uint16_t releaseval = releasevalINIT;   //angle *2 at which and IC will be released
uint16_t pickval = pickupINIT;          //angle at which an IC will be picked up
uint8_t ICADVVal[10];                   //used to average out the IC bucket reading
uint8_t GUIDEADCVal[10];                //used to average out the guide reading
uint8_t ADCCounter = 0;                 //used to count the averaging of the ADV values
uint8_t DeliverxMany = 0;               //amount to dispense
uint8_t task = 0;                       //task number used for task management
uint8_t jam = 0;                        //indicates a jam  in the system
uint8_t RXBuffer[6];                    //used to read in data pertaining to dispensing codes
#ifdef _LCD_
uint8_t ScrollDir = 0;                  //flag for direction of lcd scrolling
uint16_t Scrollwait = 200;              //used to remember how long to wait before changing lcd
uint8_t ScrollCount = 0;                //count the position of scrolling on the lcd
uint8_t ScrollIndex = 0;                //index to where lcd must scroll too
char ScrollBuffer[256];                 //buffer containing string to scroll on the lcd
#endif
uint8_t ReportCode = SUCCESS;           //used to record how a dispense went so it can be relayed to the master
#ifdef _DIP8_
uint16_t Wait = WaitT+25;
#endif
#ifdef _DIP1416_
uint16_t Wait = WaitT + 75;
#endif
#ifdef _DIP20_
uint16_t Wait = WaitT + 150;
#endif

//function definitions
void Ready();
void delay(uint16_t time);
void AlignTaskMng();
void Calibration();
void SerialMonitor(void(*FNCName)());
void Decode();
void sendReport(uint8_t ReportCode);
uint8_t checksumcal(uint8_t *values, uint8_t checkpos);
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
void echo();
#endif
void InitWatchdog();

int main(void)
{
    //initialization functions
    GPIOInit();
    address = InitAdress();
    testflag = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
    TIM14Init();
    USART1Init();
    ADCsInit();
#ifdef _ROLLER_
    ServoInit(pickval);
    ServoSet(pickval);
    delay(1000);
    ServoZero();
#ifdef _DEBUG_
    USART2Init();
    printBIN(address);
    //input();
#endif
#ifdef _LCD_
    lcd_init(); // set up LCD lines and send initialisation commands
    lcd_command(LCD_CLEAR_DISPLAY);
    lcd_string("UCT VM Rev 1");
    lcd_command(LCD_GOTO_LINE_2); // go to lower line
    lcd_string("Nice To Meet");
#endif
    InitWatchdog();
    Ready();

#ifdef _LCD_
    PopulateBuffer("Please Swipe Your Student Card Below");
#endif

    while(1)
    {
        /* Reload IWDG counter */
        IWDG_ReloadCounter();
        //manages the serial monitor
        SerialMonitor(Decode);
        //idles when there is nothing to do in debug mode and does nothing to save power when final implementation
        Idler(period);
#ifdef _DEBUG_
        IntervalHandle(Test,200,1);
#endif
#ifdef _LCD_
//scrolled the lcd and defaults the scroll every so often
    LCDScroll();
    DefaultLCD();
#endif
        //check if the mode flag has changed
        uint8_t tempmode = modeflag;
        ModeSelect();
        //if it has changed setup for alignment
        if(modeflag != tempmode){
            ServoSet(pickval);
            task = 0;
        }
#ifdef _DEBUG_
        //debuginput();
#endif
        if(modeflag){
                //start alignment
            AlignTaskMng();
#ifdef _DEBUG_
        //if in debug mode unjam the device
            jam = 0;
#endif
        }else if(!modeflag){
#ifdef _ROLLER_
            //if mode flag not set run roller task manager
            TaskManager();
#endif
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
        IWDG_ReloadCounter();
        sum = TIM_GetCounter(TIM14) - Stamp[5];
    }
}
//task manager for alignment calibration
void AlignTaskMng(){
    switch(task){
        case 0:{
            FinishTask();                       //finishes up any task currently running
            period = 500;                       //sets blinker period to 500ms
            task ++;                            //increments the task counter
            Stamp[3] = TIM_GetCounter(TIM14);   //resets the alignment timer
            break;
        }
        case 1:{
            //starts alignment calibration
            Calibration();
            break;
        }
        default:{
            //when finished servo sent back to pickup value
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
    //runs release alignment for 15 seconds
    if(sum <= 15000){
        //averages the adc value by reading it in 10 times over 200ms at 20ms intervals
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 20){
        ICADVVal[ADCCounter] = GetADCVal(channel0);     //save adc value
        ADCCounter++;
            if(ADCCounter >= 10){
                ADCCounter = 0;
                int i;
                uint16_t ICSum = 0;
                for(i = 0; i < 10; i++){                //avarages the adc value
                    ICSum += ICADVVal[i];
                }
                ICSum = (ICSum/10)/2;
            #ifdef _EXTRA_
                print16bits(ICSum,0x41,3);
            #endif
                //decides whether to add or subtract from current release value
                if(ICSum > 64){
                    ICSum -= 32;
                    releaseval = releasevalINIT + ICSum;
                }else if(ICSum == 64){
                    ICSum = 0;
                }else{
                    ICSum = 31 - ICSum;
                    releaseval = releasevalINIT - ICSum;
                }
            }
        }
        #ifdef _DEBUG_
        print16bits(releaseval,0x52,3);
        #endif
        //puts new value on servo
        ServoSet(releaseval);
        //runs pickup alignment for 15s
    }else if(sum <= 30000){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 20){
        ICADVVal[ADCCounter] = GetADCVal(channel0);
        ADCCounter++;
            if(ADCCounter >= 10){
                ADCCounter = 0;
                int i;
                uint16_t ICSum = 0;
                for(i = 0; i < 10; i++){
                    ICSum += ICADVVal[i];
                }
                ICSum = (ICSum/10)/2;
            #ifdef _EXTRA_
                print16bits(ICSum,0x41,3);
            #endif

                if(ICSum > 64){
                    ICSum -= 32;
                    pickval = pickupINIT + ICSum;
                }else if(ICSum == 64){
                    ICSum = 0;
                }else{
                    ICSum = 31 - ICSum;
                    pickval = pickupINIT - ICSum;
                }
            }
        }
        #ifdef _DEBUG_
        print16bits(pickval,0x50,3);
        #endif
        ServoSet(pickval);
    }else{
        task++;
    }
}
//function used to determine if serial data received has stopped.
void SerialMonitor(void(*FNCName)()){
    //checks time last serial data was received, if greater than 5ms then no more data to be received
    uint16_t counterval = TIM_GetCounter(TIM14);
    if(RXFlag == 1){
        RXFlag = 0;
        Stamp[7] = counterval;
    }
    uint16_t sum = counterval - Stamp[7];
    if((sum >= 10) && (RXFlag != 2)){
        FNCName();
        RXFlag = 2;
    }
}
//Message Decoder from master
void Decode(){
    //if only 6 values in buffer it is a command code
    //print16bits(buffercount - bufferreadcount, 0x4d,3);
    if(buffercount - bufferreadcount == 6){
        int i;
        //transfer from serial buffer to rx reading buffer
        for(i = 0; i < 6 ; i++){
            RXBuffer[i] = Serialdata[bufferreadcount++];
        }
        //calculate checksum to make sure it is a valid package
        uint8_t check = checksumcal(RXBuffer,4);
        //check structure validity of package
        if((RXBuffer[0] == StartByte) && (RXBuffer[1] == address) && (RXBuffer[4] == check) && (RXBuffer[5] == EndByte)){
            switch(RXBuffer[2]){
                //send black response to master
                case CALL:{
                    sendReport(SUCCESS);
                    break;
                }
                //start dispensing
                case DISPENSE:{
                    if(!jam){
                        task = 1;
                        DeliverxMany = RXBuffer[3];
                        Stamp[9] = TIM_GetCounter(TIM14);
                    }else{
                        sendReport(JAM);
                    }
                    break;
                }
                //release jam flag
                case FREE:{
                    jam = 0;
                    ServoSet(pickval);
                    sendReport(SUCCESS);
                    delay(1000);
                    ServoZero();
                    break;
                }
                default:{
                    #ifdef _DEBUG_
                    printbyte(0x46);
                    #endif
                    break;
                }
            }
        }
    }
    #ifdef _LCD_
    else{   //if buffer contains more than 6 bytes then LCD command
        uint8_t lcdbuffer = buffercount - bufferreadcount;
        uint8_t NewSCroll = lcdbuffer - 5;
        char UpdateToo[NewSCroll];
        int i;
        //transferring serial data the rx buffer then string to print into a separate buffer
        for(i = 0; i < lcdbuffer ; i++){
            RXBuffer[i] = Serialdata[bufferreadcount++];

            if(i > 2 && i < lcdbuffer - 2){
                UpdateToo[i - 3] = RXBuffer[i];
                UpdateToo[i - 2] = 0;
            }

        }
        //checking validity of the package
        uint8_t check = checksumcal(RXBuffer,lcdbuffer - 2);
        if((RXBuffer[0] == StartByte) && (RXBuffer[1] == address) && (RXBuffer[2] == LCDPRINT) && (RXBuffer[lcdbuffer - 1] == EndByte)){//(RXBuffer[lcdbuffer - 2] == check) &&
            PopulateBuffer(UpdateToo);
            sendReport(SUCCESS);
        }
    }
    #else
    else{   //if buffer contains more than 6 bytes then LCD command
        //printbyte(0x4C);
        //printbyte(0x43);
        //printbyte(0x44);
        uint8_t lcdbuffer = buffercount - bufferreadcount;
        //transferring serial data the rx buffer then string to print into a separate buffer
        int i;
        for(i = 0; i < lcdbuffer ; i++){
            bufferreadcount++; //flush the buffer
        }
    }
    #endif
}
//checksum calculator
uint8_t checksumcal(uint8_t *values, uint8_t checkpos){
	uint8_t check = 0;
	int i;
	for(i = 0 ; i < checkpos ; i++){
        check = (check + *values++) & 0xFF;
	}
	return check;
}

//reports to send back to master
void sendReport(uint8_t ReportCode){
    //GPIO_WriteBit(GPIOA,RE,1);//disable receiving of data
    GPIO_WriteBit(GPIOA,DE,1);//enable sending on the rs485 bus

    delay(10);

    USART_SendData(USART1, 0xD1);//start byte
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}

    USART_SendData(USART1, address);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}

    USART_SendData(USART1, ReportCode);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}

    USART_SendData(USART1, 0x01);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}

    uint8_t check = 0;
    check = (check + 0xD1) & 0xFF;
    check = (check + address) & 0xFF;
    check = (check + ReportCode) & 0xFF;
    check = (check + 0x01) & 0xFF;

    USART_SendData(USART1, check);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}

    USART_SendData(USART1, 0xE1);//stop byte
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}

    delay(10);

    GPIO_WriteBit(GPIOA,DE,0); //disbale sending on the rs485 bus
    GPIO_WriteBit(GPIOA,RE,0);//enable receiving of data
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
            //init of dispensary task
            initTask();
            break;
        }
        case 2:{
            //check if there an IC in the bucket
            CheckIC();
            break;
        }
        case 3:{
            //release the IC
            Release();
            break;
        }
        case 4:{
            //wiggle the bucket if IC is stuck
            ServoWiggle(ReleaseWiggle,releaseval);
            break;
        }
        case 5:{
            //check it is released
            Release();
            break;
        }
        case 6:{
            //return to pickup
            PickUp();
            break;
        }
        case 7:{
            //wiggle if no IC
            ServoWiggle(PickWiggle,pickval);
            break;
        }
        case 8:{
            //final IC check
            CheckIC2();
            break;
        }
        case 9:{
            //final check, if tube is low, only runs on last dispense
            if(DeliverxMany == 1){
                CheckLow();
            }else{
                task++;
            }
            break;
        }
        default:{
            //finish of dispense or setup to run again
            DeliverxMany--;
            if(DeliverxMany != 0){
                task = 1;
                break;
            }
#ifdef _DEBUG_
            if(!jam){
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
    ReportCode = SUCCESS;               //set report code to default value
    ServoSet(pickval);                  //set servo to pickup value
    period = 100;                       //set led blik period to 100ms
    //GPIO_WriteBit(GPIOA,RE,1);          //turn off ability to read from rs485 bus
    GPIO_WriteBit(GPIOB,VBRMTR,1);      //turn on vibration mtr
    GPIO_WriteBit(GPIOB,IRLED1,1);      //turn on IR LED
    GPIO_WriteBit(GPIOB,IRLED2,1);      //turn on IR LED
    task++;                             //send to next task
    Stamp[2] = TIM_GetCounter(TIM14);   //reset task counter
}

//check for IC in bucket and tube then move to release
void CheckIC(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[2];
    //waits some time before starting for system to settle
    if(sum >= Wait){
        uint16_t sum2 = counterval - Stamp[4];
        //averages adc values
        if(sum2 >= 20){
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
#ifdef _DEBUG_
                print16bits(ICSum,0x41,3);
                print16bits(GuideSum,0x42,3);
#endif
                //checks the adc value and determines the status
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
                    ReportCode = JAM;
                    jam = 1;
                    DeliverxMany = 1;
                    task = 0xFF;
                }else{  //tube is empty
                    //report empty
                    #ifdef _DEBUG_
                    printbyte(0x45);
                    #endif
                    ReportCode = EMPTY;
                    DeliverxMany = 1;
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
    //waits for system to settle
    if(sum >= Wait){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 20){
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
                //checks adc value and determines status of system
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
                    ReportCode = JAM;
                    jam = 1;
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
    if(sum >= Wait){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 20){
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
#ifdef _DEBUG_
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

//see above for comments for similar code
//checks IC but does not end task if empty
void CheckIC2(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[2];
    if(sum >= Wait){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 20){
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
                    if(DeliverxMany == 1)printbyte(0x53);;
                    #endif
                    ReportCode = JAM;
                    jam = 1;
                    DeliverxMany = 1;
                    task = 0xFF;
                }else{  //tube is empty
                    //report empty
                    #ifdef _DEBUG_
                    printbyte(0x45);
                    if(DeliverxMany == 1)printbyte(0x53);
                    #endif
                    if(DeliverxMany == 1){
                        ReportCode |= EMPTY;
                    }else{
                        ReportCode = EMPTY;
                        DeliverxMany = 1;
                    }
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
    if(sum >= Wait){
        uint16_t sum2 = counterval - Stamp[4];
        if(sum2 >= 20){
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
                    ReportCode |= LOW;
                    task++;
                }
            }
            Stamp[4] = counterval;
        }
    }
}
//similar code done

//finish off the task and back to idle
void FinishTask(){
    GPIO_WriteBit(GPIOA,RE,0);          //enable receiving of data again
    GPIO_WriteBit(GPIOB,VBRMTR,0);      //turn off vibration mtr
    GPIO_WriteBit(GPIOB,IRLED1,0);      //turn off IR LED
    GPIO_WriteBit(GPIOB,IRLED2,0);      //turn off IR LED
    task = 0;                           //set task counter to 0
    period = 1000;                      //set led blink period to 1s
    ServoZero();                        //stop holding servo
    sendReport(ReportCode);             //send report
    uint16_t timetaken = TIM_GetCounter(TIM14) - Stamp[9];
    #ifdef _DEBUG_
    print16bits(timetaken,0x54,5);
    #endif
}

//wiggle the servo by the specified amount
void ServoWiggle(uint8_t pos, uint16_t initpos){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[5];
    uint16_t waitfor = 50;
    //change position every 75 ms after the first movement
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
        //set to initial position
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
#ifdef _DEBUG_
#else
    if(task > 0){
#endif
        uint16_t counterval = TIM_GetCounter(TIM14);
        uint16_t sum = counterval - Stamp[0];
        if(sum >= period){
            status ^= 1;
            GPIO_WriteBit(GPIOB,GPIO_Pin_9,1);
            GPIO_WriteBit(GPIOB,GPIO_Pin_8,status);
            Stamp[0] = counterval;
        }
#ifdef _DEBUG_
#else
    }else{
        GPIO_WriteBit(GPIOB,GPIO_Pin_9,0);
        GPIO_WriteBit(GPIOB,GPIO_Pin_8,0);
    }
#endif
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
dispensary test using address switch 8, delivers 2 components
be careful when using not to leave it on an unknown address
*/
void Test(){
    uint8_t tempbit = testflag;
    testflag = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
    if(tempbit != testflag){
        DeliverxMany = deliverytest;
#ifdef _EXTRA_
        print16bits(DeliverxMany,0x43,3);
#endif
        jam = 0;
        task = 1;
    }
}

#ifdef _DEBUG_
/*
debug input function
*/
void debuginput(){
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)){
        DeliverxMany = 1;
        task = 1;
    }else if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)){
        //task = 0;
    }
}

//irq handler
void USART2_IRQHandler(){
    /* Read one byte from the receive data register */
    Serialdata[buffercount] = USART2->RDR;
    buffercount++;
    RXFlag = 1;
    //USART_ClearFlag(USART2,USART_FLAG_RXNE);
}

//echo test to test usarts rolling buffer and serial monitor function
void echo(){
    while(bufferreadcount != buffercount){
        USART_SendData(USART2, Serialdata[bufferreadcount]);
        bufferreadcount++;
        /* Loop until the end of transmission */
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}

    }
    USART_SendData(USART2, 0x0A);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
    #ifdef _EXTRA_
    print16bits(buffercount,0x41,3);
    #endif
}
#endif

//communications management

//usart 1 Interrupt handler
void USART1_IRQHandler(){
    Serialdata[buffercount] = USART1->RDR;
    buffercount++;
    RXFlag = 1;
}

#ifdef _LCD_
//lcd scrolling function
void LCDScroll(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[8];
    //wait scrollwait time before changing LCD
    if(sum >= Scrollwait){
        Scrollwait = 350;
        //update index based on value of the scroll count
        if(ScrollDir == 0){
            ScrollIndex = ScrollIndex + 1;
            if(ScrollIndex == ScrollCount - 32){
                ScrollDir = 1;
                Scrollwait = 2000;
            }
        }else{
            ScrollIndex = ScrollIndex - 1;
            if(ScrollIndex == 0){
                ScrollDir = 0;
                Scrollwait = 2000;
            }
        }
        //update buffer based on the index
        char lineone[16];
        char linetwo[16];
        int i;
        for(i = 0; i < 16; i++){
            lineone[i] = ScrollBuffer[ScrollIndex + i];
        }
        for(i = 0; i < 16; i++){
            linetwo[i] = ScrollBuffer[ScrollIndex + 16 + i];
        }
        if(ScrollCount <= 32){
            for(i = 0; i < 16; i++){
                lineone[i] = ScrollBuffer[i];
                linetwo[i] = ScrollBuffer[16 + i];
            }
        }
        lcd_command(LCD_CLEAR_DISPLAY);
        lcd_string(lineone);
        lcd_command(LCD_GOTO_LINE_2);
        lcd_string(linetwo);
        Stamp[8] = counterval;
    }
}

//insert into scrolling buffer
void PopulateBuffer(uint8_t *String_to_insert){

    ScrollIndex = 0;
    ScrollDir = 0;
    Stamp[8] = TIM_GetCounter(TIM14);
    Stamp[9] = TIM_GetCounter(TIM14);
    int i;
    //put spaces in the buffer so it isnt holding old values
    for(i = 0; i < 256; i ++){
        ScrollBuffer[i] = 0x20;
    }
    uint32_t counter=0;
    //insert new values into buffer until empty
    while (String_to_insert[counter] != 0) {
        ScrollBuffer[counter] = String_to_insert[counter];
        counter = counter + 1;
        ScrollCount = counter;
    }
    //puts initial values on the lcd
    char lineone[16];
    char linetwo[16];
    for(i = 0; i < 16; i++){
        lineone[i] = ScrollBuffer[i];
        linetwo[i] = ScrollBuffer[16 + i];
    }
    lcd_command(LCD_CLEAR_DISPLAY);
    lcd_string(lineone);
    lcd_command(LCD_GOTO_LINE_2);
    lcd_string(linetwo);
    Scrollwait = 2000;
}

void DefaultLCD(){
    uint16_t counterval = TIM_GetCounter(TIM14);
    uint16_t sum = counterval - Stamp[9];
    //after 60 seconds of populating the buffer the lcd will be defaulted
    if(sum >= 60000){
        PopulateBuffer("Please Swipe Your Student Card Below");
        Stamp[9] = counterval;
    }
}
#endif

//watchdog setup
void InitWatchdog(){
     /* IWDG timeout equal to 250ms.
    The timeout may varies due to LSI frequency dispersion, the
    LSE value is centred around 32 KHz */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    /* Set counter reload value to obtain 250ms IWDG TimeOut.
    Counter Reload Value = 250ms/IWDG counter clock period
    = 250ms / (LSI/32)
    = 0.25s / (32 KHz /32)
    = 250
    */
    IWDG_SetReload(250);
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
    /* Enable IWDG (LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}


