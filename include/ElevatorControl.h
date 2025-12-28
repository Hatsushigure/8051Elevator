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

typedef enum
{
    EDS_Closed,
    EDS_Closing,
    EDS_Open
} ElevatorDoorState;

typedef struct
{
    ElevatorRunState runState;
    ElevatorDoorState doorState;
    uint8_t currentFloorIndex;
    uint8_t requestBitmap[10];
} ElevatorControl;

extern ElevatorControl elevatorControl;

void ElevatorControl_init();
void ElevatorControl_makeRequest(int8_t floorIndex, FloorRequest request);
void ElevatorControl_move();
int8_t ElevatorControl_indexToFloor(int8_t index);

#endif // __ELEVATOR_CONTROL_H__
