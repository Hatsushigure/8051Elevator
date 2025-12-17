#include "ElevatorControl.h"

ElevatorControl elevatorControl;

void openDoor() {}

uint8_t floorToIndex(int8_t floor)
{
    floor += (floor < 0);
    return floor + 1;
}

bit hasRequestAbove(uint8_t floorIndex)
{
    uint8_t i = floorIndex + 1;
    for (; i != 10; i++)
        return (elevatorControl.requestBitmap[i]);
    return 0;
}

bit hasRequestBelow(uint8_t floorIndex)
{
    int8_t i = floorIndex - 1;
    for (; i >= 0; i--)
        return (elevatorControl.requestBitmap[i]);
    return 0;
}

void ElevatorControl_makeRequest(int8_t floor, FloorRequest request)
{
    elevatorControl.requestBitmap[floorToIndex(floor)] |= request;
}

void ElevatorControl_move()
{
    switch (elevatorControl.runState)
    {
    case ERS_Idle:
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor])
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] =
                FR_None;
            openDoor();
        } else if (hasRequestAbove(elevatorControl.currentFloor))
            elevatorControl.runState = ERS_MovingUp;
        else if (hasRequestBelow(elevatorControl.currentFloor))
            elevatorControl.runState = ERS_MovingDown;
        break;
    case ERS_MovingUp:
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor] &
            (FR_Up | FR_Inside))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] &=
                (~(FR_Up | FR_Inside));
            openDoor();
            if (hasRequestAbove(elevatorControl.currentFloor))
                break;
            elevatorControl.runState = ERS_Idle;
            break;
        }
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor] &&
            !hasRequestAbove(elevatorControl.currentFloor))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] &=
                (~FR_Down);
            openDoor();
            if (hasRequestBelow(elevatorControl.currentFloor))
            {
                elevatorControl.runState = ERS_MovingDown;
                break;
            }
            elevatorControl.runState = ERS_Idle;
            break;
        }
    case ERS_MovingDown:
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor] &
            (FR_Down | FR_Inside))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] &=
                (~(FR_Down | FR_Inside));
            openDoor();
            if (hasRequestBelow(elevatorControl.currentFloor))
                break;
            elevatorControl.runState = ERS_Idle;
            break;
        }
        if (elevatorControl.requestBitmap[elevatorControl.currentFloor] &&
            !hasRequestBelow(elevatorControl.currentFloor))
        {
            elevatorControl.requestBitmap[elevatorControl.currentFloor] &=
                (~FR_Up);
            openDoor();
            if (hasRequestAbove(elevatorControl.currentFloor))
            {
                elevatorControl.runState = ERS_MovingUp;
                break;
            }
            elevatorControl.runState = ERS_Idle;
            break;
        }
    }
}
