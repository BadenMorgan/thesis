#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

//status bytes
#define JAM 0xC1
#define EMPTY 0xC2
#define LOW 0xC4
#define SUCCESS 0xC8
//command bytes
#define CALL 0xB1
#define DISPENSE 0xB3
#define FREE 0xB5
#define LCDPRINT 0xB7

#define StartByte 0xA1
#define EndByte 0xF1

//program macros
#define Wait 250    //Time between each dispensary task
#define GapThreshold 150

#define channel0 0
#define channel8 1
#define channel9 2

#define PickWiggle 10
#define ReleaseWiggle 15

#define releasevalINIT 144
#define pickupINIT 283



#endif /* DEFINITIONS_H_INCLUDED */
