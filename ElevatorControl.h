#ifndef __ELEVATOR_CONTROL_H__
#define __ELEVATOR_CONTROL_H__

#include "utils.h"

typedef enum
{
    ERS_Idle,
    ERS_MovingUp,
    ERS_MovingDown
} ElevatorRunState;

typedef enum
{
    FR_None = 0x00,
    FR_Up = 0x01,
    FR_Down = 0x02,
    FR_Inside = 0x04
} FloorRequest;

typedef struct
{
    ElevatorRunState runState;
    uint8_t currentFloor;
    uint8_t requestBitmap[10];
} ElevatorControl;

extern ElevatorControl elevatorControl;

#define ElevatorControl_init()                                                 \
    {                                                                          \
        uint8_t i = 0;                                                         \
        elevatorControl.runState = ERS_Idle;                                   \
        elevatorControl.currentFloor = 0;                                      \
        for (; i != 10; i++)                                                   \
            elevatorControl.requestBitmap[i] = 0;                              \
    }
void ElevatorControl_makeRequest(int8_t floor, FloorRequest request);
void ElevatorControl_move();

#endif // __ELEVATOR_CONTROL_H__
