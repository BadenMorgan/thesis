#ifndef GPIOLIB_H_INCLUDED
#define GPIOLIB_H_INCLUDED
#include "stm32f0xx_conf.h"
#define MCUON GPIO_Pin_9
#define MCUSYS GPIO_Pin_8
#define IRLED1 GPIO_Pin_2
#define IRLED2 GPIO_Pin_10
#define VBRMTR GPIO_Pin_13
#define DE GPIO_Pin_8
#define RE GPIO_Pin_11
#define MODE GPIO_Pin_12

void GPIOInit();
uint8_t InitAdress();
void ADCsInit();

#endif /* GPIOLIB_H_INCLUDED */
