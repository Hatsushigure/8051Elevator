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

#define ElevatorDoorTextBlinkCounterDefault 5
#define ElevatorOpenDoorCounterDefault 10
#define ElevatorCloseDoorCounterDefault 4
#define ElevatorMoveCounterDefault 5
#define FinishDelayTimeInitial 100
#define WrongPasswordDelayTimeInitial 100
#define UpdateElevatorStatusCounterInitial 10
extern const char code PasswordPrompt[];
extern const char code PersonPrompt[];
extern const char code WeightPrompt[];
extern const char code TrialCountPrompt[];
extern const char code FinishPrompt[];
extern const char code PasswordWrongPrompt[];
extern const uint8_t code CorrectPassword[];
extern const char code OpenDoorPrompt[];
extern const char code CloseDoorPrompt[];
extern const char code DisableFloorPrompt[];
extern const char code AskConfigDisableFloorPrompt[];

void myItoa(int8_t value, char* str);

#endif // __UTILS_H__
