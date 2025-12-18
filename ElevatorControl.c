#include "ElevatorControl.h"

ElevatorControl elevatorControl;

#define floorToIndex(floor) (floor + 3)

uint8_t hasRequestAbove(uint8_t floorIndex)
{
    uint8_t i = floorIndex + 1;
    for (; i != 12; i++)
    {
        if (elevatorControl.requestBitmap[i])
            return i;
    }
    return 0;
}

uint8_t hasRequestBelow(uint8_t floorIndex)
{
    int8_t i = floorIndex - 1;
    for (; i >= 0; i--)
    {
        if (elevatorControl.requestBitmap[i])
            return i;
    }
    return 0;
}

void ElevatorControl_makeRequest(int8_t floor, FloorRequest request)
{
    elevatorControl.requestBitmap[floorToIndex(floor)] |= request;
}

void ElevatorControl_move()
{
    if (elevatorControl.doorState != EDS_Closed)
        return;
    switch (elevatorControl.runState)
    {
    case ERS_Idle:
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor])
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] =
                FR_None;
            elevatorControl.doorState = EDS_Open;
        } else if (elevatorControl.targetFloor =
                       hasRequestAbove(elevatorControl.currentFloor))
            elevatorControl.runState = ERS_MovingUp;
        else if (elevatorControl.targetFloor =
                     hasRequestBelow(elevatorControl.currentFloor))
            elevatorControl.runState = ERS_MovingDown;
        break;
    case ERS_MovingUp:
        elevatorControl.currentFloor++;
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor] &
            (FR_Up | FR_Inside))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] &=
                (~(FR_Up | FR_Inside));
            elevatorControl.doorState = EDS_Open;
            if (elevatorControl.targetFloor =
                    hasRequestAbove(elevatorControl.currentFloor))
                break;
            elevatorControl.runState = ERS_Idle;
            break;
        }
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor] &&
            !hasRequestAbove(elevatorControl.currentFloor))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] &=
                (~FR_Down);
            elevatorControl.doorState = EDS_Open;
            if (elevatorControl.targetFloor =
                    hasRequestBelow(elevatorControl.currentFloor))
            {
                elevatorControl.runState = ERS_MovingDown;
                break;
            }
            elevatorControl.runState = ERS_Idle;
            break;
        }
        break;
    case ERS_MovingDown:
        elevatorControl.currentFloor--;
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor] &
            (FR_Down | FR_Inside))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] &=
                (~(FR_Down | FR_Inside));
            elevatorControl.doorState = EDS_Open;
            if (elevatorControl.targetFloor =
                    hasRequestBelow(elevatorControl.currentFloor))
                break;
            elevatorControl.runState = ERS_Idle;
            break;
        }
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor] &&
            !hasRequestBelow(elevatorControl.currentFloor))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] &=
                (~FR_Up);
            elevatorControl.doorState = EDS_Open;
            if (elevatorControl.targetFloor =
                    hasRequestAbove(elevatorControl.currentFloor))
            {
                elevatorControl.runState = ERS_MovingUp;
                break;
            }
            elevatorControl.runState = ERS_Idle;
            break;
        }
        break;
    }
}
