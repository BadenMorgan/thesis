#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

//status bytes
#define JAM 0xC1
#define EMPTY 0xC3
#define LOW 0xC5
#define SUCCESS 0xC7
#define LOWSUCCESS 0xC9
//command bytes
#define CALL 0xA1
#define DISPENSE 0xA3
#define FREE 0xA5

//program macros
#define Wait 550    //Time between each dispensary task
#define GapThreshold 150

#define channel0 0
#define channel8 1
#define channel9 2

#define PickWiggle 5
#define ReleaseWiggle 15



#endif /* DEFINITIONS_H_INCLUDED */
