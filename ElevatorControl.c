#include "ElevatorControl.h"

ElevatorControl elevatorControl;

int8_t ElevatorControl_indexToFloor(int8_t index)
{
    index -= 2;
    return index - (index <= 0);
}

uint8_t hasRequestAbove(uint8_t floorIndex)
{
    uint8_t i = floorIndex + 1;
    for (; i != 11; i++)
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

void ElevatorControl_makeRequest(int8_t floorIndex, FloorRequest request)
{
    elevatorControl.requestBitmap[floorIndex] |= request;
}

void ElevatorControl_move()
{
    if (elevatorControl.doorState != EDS_Closed)
        return;
    switch (elevatorControl.runState)
    {
    case ERS_Idle:
        if (elevatorControl.requestBitmap[elevatorControl.currentFloorIndex])
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloorIndex] =
                FR_None;
            elevatorControl.doorState = EDS_Open;
        } else if (elevatorControl.targetFloorIndex =
                       hasRequestAbove(elevatorControl.currentFloorIndex))
            elevatorControl.runState = ERS_MovingUp;
        else if (elevatorControl.targetFloorIndex =
                     hasRequestBelow(elevatorControl.currentFloorIndex))
            elevatorControl.runState = ERS_MovingDown;
        break;
    case ERS_MovingUp:
        elevatorControl.currentFloorIndex++;
        if (elevatorControl.requestBitmap[elevatorControl.currentFloorIndex] &
            (FR_Up | FR_Inside))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloorIndex] &=
                (~(FR_Up | FR_Inside));
            elevatorControl.doorState = EDS_Open;
            if (elevatorControl.targetFloorIndex =
                    hasRequestAbove(elevatorControl.currentFloorIndex))
                break;
            elevatorControl.runState = ERS_Idle;
            break;
        }
        if (elevatorControl.requestBitmap[elevatorControl.currentFloorIndex] &&
            !hasRequestAbove(elevatorControl.currentFloorIndex))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloorIndex] &=
                (~FR_Down);
            elevatorControl.doorState = EDS_Open;
            if (elevatorControl.targetFloorIndex =
                    hasRequestBelow(elevatorControl.currentFloorIndex))
            {
                elevatorControl.runState = ERS_MovingDown;
                break;
            }
            elevatorControl.runState = ERS_Idle;
            break;
        }
        break;
    case ERS_MovingDown:
        elevatorControl.currentFloorIndex--;
        if (elevatorControl.requestBitmap[elevatorControl.currentFloorIndex] &
            (FR_Down | FR_Inside))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloorIndex] &=
                (~(FR_Down | FR_Inside));
            elevatorControl.doorState = EDS_Open;
            if (elevatorControl.targetFloorIndex =
                    hasRequestBelow(elevatorControl.currentFloorIndex))
                break;
            elevatorControl.runState = ERS_Idle;
            break;
        }
        if (elevatorControl.requestBitmap[elevatorControl.currentFloorIndex] &&
            !hasRequestBelow(elevatorControl.currentFloorIndex))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloorIndex] &=
                (~FR_Up);
            elevatorControl.doorState = EDS_Open;
            if (elevatorControl.targetFloorIndex =
                    hasRequestAbove(elevatorControl.currentFloorIndex))
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
