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
//protocol bytes
#define StartByte 0xA1
#define EndByte 0xF1

//program macros
#define Wait 250            //Time between each dispensary task
#define GapThreshold 150    //threshold before event is triggered


#define channel0 0
#define channel8 1
#define channel9 2

#define PickWiggle 10       //wiggle rate for picking up
#define ReleaseWiggle 15    //wiggle rate for releasing

#define releasevalINIT 144  //servo position to set the release
#define pickupINIT 283      //servo value to set the pickup



#endif /* DEFINITIONS_H_INCLUDED */
