#include "ElevatorControl.h"

ElevatorControl elevatorControl;

int8_t ElevatorControl_indexToFloor(int8_t index)
{
    index--;
    return index - (index <= 0);
}

bit hasRequestAbove(uint8_t floorIndex)
{
    uint8_t i = floorIndex + 1;
    for (; i != 10; i++)
    {
        if (elevatorControl.requestBitmap[i])
            return 1;
    }
    return 0;
}

bit hasRequestBelow(uint8_t floorIndex)
{
    int8_t i = floorIndex - 1;
    for (; i >= 0; i--)
    {
        if (elevatorControl.requestBitmap[i])
            return 1;
    }
    return 0;
}

void ElevatorControl_init()
{
    uint8_t i = 0;
    elevatorControl.runState = ERS_Idle;
    elevatorControl.doorState = EDS_Closed;
    elevatorControl.currentFloorIndex = 2;
    for (; i != 10; i++)
    {
        elevatorControl.requestBitmap[i] = 0;
        elevatorControl.floorEnableStatus[i] = 1;
    }
}

void ElevatorControl_makeRequest(int8_t floorIndex, FloorRequest request)
{
    if (!elevatorControl.floorEnableStatus[floorIndex])
        return;
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
        } else if (hasRequestAbove(elevatorControl.currentFloorIndex))
            elevatorControl.runState = ERS_MovingUp;
        else if (hasRequestBelow(elevatorControl.currentFloorIndex))
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
            if (hasRequestAbove(elevatorControl.currentFloorIndex))
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
            if (hasRequestBelow(elevatorControl.currentFloorIndex))
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
            if (hasRequestBelow(elevatorControl.currentFloorIndex))
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
            if (hasRequestAbove(elevatorControl.currentFloorIndex))
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
