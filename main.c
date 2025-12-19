#include "ElevatorControl.h"
#include "Joystick.h"
#include "LcdDisplay.h"
#include "Led.h"
#include "NumberInput.h"
#include "display.h"
#include "initialize.h"
#include "keyboard.h"
#include "utils.h"

#ifdef __VSCODE_C51__
#define INTERRUPT(x)
#else
#define INTERRUPT(x) interrupt x
#endif

typedef enum
{
    WS_GetPassword,
    WS_VarifyPassword,
    WS_PasswordWrong,
    WS_GetMaxPerson,
    WS_GetMaxWeight,
    WS_Finish,
    WS_GetElevatorControl,
    WS_ProcessElevatorControl,
    WS_Free
} WorkState;

const uint8_t code castTable[] = {DC_0,        DC_1, DC_2,        DC_3,
                                  DC_4,        DC_5, DC_6,        DC_7,
                                  DC_8,        DC_9, DS_Middle,   DC_B,
                                  DC_C,        DC_D, DC_E,        DC_F,
                                  DS_Disabled, DC_D, DS_Disabled, DC_U};
const uint8_t code correctPassword[] = {1, 1, 4, 5, 1, 4};
const uint8_t code personPrompt[] = {DC_N, DC_O& DS_Dot};
const uint8_t code weightPrompt[] = {DC_R, DC_L& DS_Dot};
const uint8_t code finishPrompt[] = {DC_F, DC_1, DC_N, DC_1, DC_5, DC_H};
const uint8_t code errorPrompt[] = {DC_E, DC_R, DC_R};
const uint8_t code openDoorPrompt[] = "OP";
const uint8_t code closeDoorPrompt[] = "CL";
#define ElevatorDoorTextBlinkCounterDefault 25
#define ElevatorOpenDoorCounterDefault 10
#define ElevatorCloseDoorCounterDefault 4
#define ElevatorMoveCounterDefault 50

WorkState workState = WS_GetPassword;
uint8_t passwordTrialCount = 5;
uint8_t wrongPasswordDelayTime;
uint8_t finishDelayTime = 80;
uint8_t maxPerson;
uint8_t maxWeight;
bit configurationComplete = 0;
uint8_t elevatorDoorTextBlinkCounter =
    ElevatorDoorTextBlinkCounterDefault; // Used to set a 0.5 s blink
uint8_t elevatorOpenDoorCounter =
    ElevatorOpenDoorCounterDefault; // Used to keep door open for 5s
uint8_t elevatorCloseDoorCounter =
    ElevatorCloseDoorCounterDefault; // Used to keep door closing for 2s
uint8_t elevatorMoveCounter =
    ElevatorMoveCounterDefault; // Elevator floor changes every 1s
bit elevatorDoorTextVisible = 1;

void getContent(
    uint8_t contentLength,
    uint8_t promptLength,
    const uint8_t code* nextPrompt,
    uint8_t nextPromptSize,
    uint8_t nextState,
    bit isPassword,
    bit isElevatorControl
);
#define getPassword()                                                          \
    {                                                                          \
        getContent(6, 0, 0, 0, WS_VarifyPassword, 1, 0);                       \
    }
#define getMaxPerson()                                                         \
    {                                                                          \
        getContent(3, 2, weightPrompt, 2, WS_GetMaxWeight, 0, 0);              \
        maxWeight = numberInput.result;                                        \
    }
#define getMaxWeight()                                                         \
    {                                                                          \
        getContent(3, 2, finishPrompt, 6, WS_Finish, 0, 0);                    \
        maxPerson = numberInput.result;                                        \
    }
#define getElevatorControl()                                                   \
    {                                                                          \
        getContent(2, 0, 0, 0, WS_ProcessElevatorControl, 0, 1);               \
    }
void varifyPassword();
void wrongPasswordDelay();
void finishDelay();
void processElevatorControl();

int main()
{
    init();
    while (1)
        ;
}

void onTimer0Timeout() INTERRUPT(1)
{
    EA = 0;
    refillTimer0();
    switch (workState)
    {
    case WS_GetPassword:
        getPassword();
        break;
    case WS_VarifyPassword:
        varifyPassword();
        break;
    case WS_PasswordWrong:
        wrongPasswordDelay();
        break;
    case WS_GetMaxPerson:
        getMaxPerson();
        break;
    case WS_GetMaxWeight:
        getMaxWeight();
        break;
    case WS_Finish:
        finishDelay();
        break;
    case WS_GetElevatorControl:
        getElevatorControl();
        break;
    case WS_ProcessElevatorControl:
        processElevatorControl();
        break;
    case WS_Free:
        break;
    }
    Display_refreshDisplay();
    EA = 1;
}

void onTimer1Timeout() INTERRUPT(3)
{
    char floorStr[2];
    EA = 0;
    refillTimer1();
    if (!configurationComplete)
    {
        EA = 1;
        return;
    }
    elevatorMoveCounter--;
    if (elevatorMoveCounter == 0)
    {
        elevatorMoveCounter = ElevatorMoveCounterDefault;
        ElevatorControl_move();
    }
    LcdDisplay_fastClearLine(0);
    LcdDisplay_setCursorPos(0x40);
    myItoa(
        ElevatorControl_indexToFloor(elevatorControl.currentFloorIndex),
        floorStr
    );
    LcdDisplay_sendString(floorStr, 2);
    if (elevatorControl.doorState == EDS_Open)
    {
        Led_allOff();
        if (elevatorDoorTextVisible)
        {
            LcdDisplay_setCursorPos(5);
            LcdDisplay_sendString(openDoorPrompt, 2);
        }
        LcdDisplay_setCursorPos(8);
        myItoa(
            ElevatorControl_indexToFloor(elevatorControl.currentFloorIndex),
            floorStr
        );
        LcdDisplay_sendString(floorStr, 2);
        elevatorDoorTextBlinkCounter--;
        if (elevatorDoorTextBlinkCounter == 0)
        {
            elevatorDoorTextBlinkCounter = ElevatorDoorTextBlinkCounterDefault;
            elevatorOpenDoorCounter--;
            elevatorDoorTextVisible = !elevatorDoorTextVisible;
        }
        if (elevatorOpenDoorCounter == 0)
        {
            elevatorControl.doorState = EDS_Closing;
            elevatorOpenDoorCounter = ElevatorOpenDoorCounterDefault;
            elevatorDoorTextBlinkCounter = ElevatorDoorTextBlinkCounterDefault;
            elevatorDoorTextVisible = 1;
        }
    } else if (elevatorControl.doorState == EDS_Closing)
    {
        if (elevatorDoorTextVisible)
        {
            LcdDisplay_setCursorPos(5);
            LcdDisplay_sendString(closeDoorPrompt, 2);
        }
        elevatorDoorTextBlinkCounter--;
        if (elevatorDoorTextBlinkCounter == 0)
        {
            elevatorDoorTextBlinkCounter = ElevatorDoorTextBlinkCounterDefault;
            elevatorCloseDoorCounter--;
            elevatorDoorTextVisible = !elevatorDoorTextVisible;
        }
        if (elevatorCloseDoorCounter == 0)
        {
            elevatorCloseDoorCounter = ElevatorCloseDoorCounterDefault;
            elevatorDoorTextBlinkCounter = ElevatorDoorTextBlinkCounterDefault;
            elevatorDoorTextVisible = 1;
            elevatorControl.doorState = EDS_Closed;
            Led_allOn();
        }
    } else
    {
        if (elevatorControl.runState != ERS_Idle)
        {
            LcdDisplay_setCursorPos(6);
            if (elevatorControl.runState == ERS_MovingUp)
                LcdDisplay_sendData('^');
            else
                LcdDisplay_sendData('v');
            myItoa(
                ElevatorControl_indexToFloor(elevatorControl.targetFloorIndex),
                floorStr
            );
            LcdDisplay_setCursorPos(8);
            LcdDisplay_sendString(floorStr, 2);
        }
    }
    EA = 1;
}

void varifyPassword()
{
    uint8_t i = 6;
    bit passwordIsCorrect = 1;
    passwordTrialCount--;
    if (numberInput.currentIndex != 6)
        passwordIsCorrect = 0;
    else
    {
        while (i)
        {
            i--;
            if (numberInput.inputBuffer[i] != correctPassword[i])
            {
                passwordIsCorrect = 0;
                break;
            }
        }
    }
    if (!passwordIsCorrect)
    {
        Display_setPrompt(errorPrompt, 3);
        display.displayBuffer[5] = castTable[passwordTrialCount];
        wrongPasswordDelayTime = 100;
        workState = WS_PasswordWrong;
    } else
    {
        Display_setPrompt(personPrompt, 2);
        NumberInput_clear();
        workState = WS_GetMaxPerson;
    }
    Display_resetDelayDisappear();
}

void wrongPasswordDelay()
{
    if (passwordTrialCount == 0)
        return;
    if (wrongPasswordDelayTime == 0)
    {
        Display_clear();
        NumberInput_clear();
        workState = WS_GetPassword;
        return;
    }
    wrongPasswordDelayTime--;
}

void finishDelay()
{
    if (finishDelayTime == 0)
    {
        Display_clear();
        configurationComplete = 1;
        Led_allOn();
        workState = WS_GetElevatorControl;
    }
    finishDelayTime--;
}

void processElevatorControl()
{
    bit isExternalRequest = 0;
    int8_t floor = 0;
    if (numberInput.inputBuffer[0] >= SK_Right)
        isExternalRequest = 1;
    floor = numberInput.inputBuffer[(uint8_t)isExternalRequest + 1];
    if (numberInput.inputBuffer[0] == SK_Up)
        ElevatorControl_makeRequest(floor, FR_Up);
    else if (numberInput.inputBuffer[0] == SK_Down)
        ElevatorControl_makeRequest(floor, FR_Down);
    else
        ElevatorControl_makeRequest(floor, FR_Inside);
    NumberInput_clear();
    workState = WS_GetElevatorControl;
}

void getContent(
    uint8_t contentLength,
    uint8_t promptLength,
    const uint8_t code* nextPrompt,
    uint8_t nextPromptSize,
    uint8_t nextState,
    bit isPassword,
    bit isElevatorControl
)
{
    Display_promptInput(promptLength + numberInput.currentIndex);
    Keyboard_getKey();
    if (isElevatorControl && !numberInput.currentIndex)
    {
        Joystick_getKey();
        if (joystick.releasedKey == SK_Up || joystick.releasedKey == SK_Down)
        {
            NumberInput_append(joystick.releasedKey);
            display.displayBuffer[0] = castTable[joystick.releasedKey];
            return;
        }
    }
    if (keyboard.state != KS_Released)
        return;
    if (keyboard.releasedKey == SK_Enter)
    {
        numberInput.result = NumberInput_getNumber();
        if (!isPassword && !isElevatorControl)
            NumberInput_clear();
        workState = nextState;
        Display_setPrompt(nextPrompt, nextPromptSize);
        return;
    }
    if (keyboard.releasedKey == SK_Backspace)
    {
        if (isElevatorControl)
        {
            display.displayBuffer[promptLength + numberInput.currentIndex] =
                DS_Disabled;
            NumberInput_backspace();
        }
        display.displayBuffer[promptLength + numberInput.currentIndex] =
            DS_Disabled;
        NumberInput_backspace();
        return;
    }
    if (numberInput.currentIndex >= contentLength)
        return;
    if (keyboard.releasedKey > 9)
        return;
    if (isElevatorControl)
    {
        int8_t floor = keyboard.releasedKey + 1;
        myItoa(ElevatorControl_indexToFloor(floor), (char*)&display.displayBuffer[numberInput.currentIndex]);
        if (display.displayBuffer[numberInput.currentIndex] == '-')
            display.displayBuffer[numberInput.currentIndex] = DS_Middle;
        else
            display.displayBuffer[numberInput.currentIndex] = DS_Disabled;
        display.displayBuffer[numberInput.currentIndex + 1] = castTable
            [display.displayBuffer[numberInput.currentIndex + 1] - '0'];
        NumberInput_append(0);
        NumberInput_append(floor);
        return;
    }
    display.displayBuffer[promptLength + numberInput.currentIndex] =
        castTable[keyboard.releasedKey];
    if (isPassword)
        Display_delayDisappear(promptLength + numberInput.currentIndex, 50);
    NumberInput_append(keyboard.releasedKey);
}
