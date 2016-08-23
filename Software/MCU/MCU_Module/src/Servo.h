#ifndef SERVO_H_INCLUDED
#define SERVO_H_INCLUDED
#include "stm32f0xx_conf.h"

void ServoInit();
void ServoSweep();
void ServoSet(uint16_t angle);

#endif /* SERVO_H_INCLUDED */
