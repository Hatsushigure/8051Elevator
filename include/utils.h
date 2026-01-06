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

#define ElevatorDoorTextBlinkCounterDefault 1
#define ElevatorOpenDoorCounterDefault                                         \
    10                               // 'OP' toogles 10 times when door open
#define ElevatorCloseDoorCounterDefault                                        \
    4                                // 'CL' toogles 4 times when door close
#define ElevatorMoveCounterDefault 2 // Elevator moves every 2 * 500ms
#define FinishDelayTimeInitial 100
#define WrongPasswordDelayTimeInitial 100
#define UpdateElevatorStatusCounterInitial                                     \
    50   // Update elevator status every 500ms
#define DoubleClickTimeCounterInitial                                          \
    30   // Two key within 300ms will be double click
#define SpecialSequenceTimeLimitInitial                                        \
    1000 // Time limit of special sequence is 10s
#define MaxPasswordTrialCount 5
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
extern const uint8_t code SpecialKeySequence[];

void myItoa(int8_t value, char* str);

#endif // __UTILS_H__
