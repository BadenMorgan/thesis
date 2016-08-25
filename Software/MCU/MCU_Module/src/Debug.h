#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED
#include "stm32f0xx_conf.h"
#include "SerialMonitor.h"
#include "Timers.h"
#include "GPIOLib.h"
#include "Servo.h"
#include "definitions.h"

void USART2Init();
void Test();
void input();
void debuginput();
void printBIN(uint8_t val);
void print16bits(uint16_t value, uint8_t pre, uint8_t len);
void printbyte(uint8_t val);
void ADCtest();

#endif /* DEBUG_H_INCLUDED */
