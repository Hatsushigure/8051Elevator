#ifndef __UTILS_H__
#define __UTILS_H__

#include <SiLABS/c8051F020.h>

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;

#define shortDelay(time)                                                       \
    while (time--)                                                             \
        ;

void refillTimer0();
void refillTimer1();

#define ElevatorDoorTextBlinkCounterDefault 25
#define ElevatorOpenDoorCounterDefault 10
#define ElevatorCloseDoorCounterDefault 4
#define ElevatorMoveCounterDefault 50
#define FinishDelayTimeInitial 100
extern const char code passwordPrompt[];
extern const char code personPrompt[];
extern const char code weightPrompt[];
extern const char code trialCountPrompt[];
extern const char code finishPrompt[];
extern const char code passwordWrongPrompt[];
extern const uint8_t code correctPassword[];
extern const char code openDoorPrompt[];
extern const char code closeDoorPrompt[];

void myItoa(int8_t value, char* str);

#endif // __UTILS_H__
