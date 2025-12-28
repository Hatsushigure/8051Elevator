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
void setupAllFloorRequestDisplay();
void updateAllFloorRequestDisplay();
void processElevatorControl();
void updateElevatorStatus();

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
    EA = 0;
    refillTimer1();
    if (!configurationComplete)
        goto onTimer1Timeout_exit;
    updateElevatorStatus();
onTimer1Timeout_exit:
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
        setupAllFloorRequestDisplay();
        workState = WS_GetElevatorControl;
    }
    finishDelayTime--;
}

void setupAllFloorRequestDisplay()
{
    uint8_t i = 1;
    LcdDisplay_setCursorPos(10);
    LcdDisplay_sendString("-2 -1", 5);
    LcdDisplay_setCursorPos(0x40);
    for (; i <= 8; i++)
    {
        LcdDisplay_sendData('0' + i);
        LcdDisplay_sendData(' ');
    }
}

void updateAllFloorRequestDisplay()
{
    uint8_t i = 0;
    for (; i < 10; i++)
    {
        if (i == 0)
            LcdDisplay_setCursorPos(12);
        else if (i == 1)
            LcdDisplay_setCursorPos(15);
        else
            LcdDisplay_setCursorPos(0x41 + (i - 2) * 2);
        if (elevatorControl.requestBitmap[i] & FR_Inside)
        {
            LcdDisplay_sendData('*');
        } else
        {
            LcdDisplay_sendData(' ');
        }
    }
}

void processElevatorControl()
{
    bit isExternalRequest = 0;
    int8_t floor = 0;
    if (numberInput.inputBuffer[0] >= SK_Right)
        isExternalRequest = 1;
    floor = numberInput.inputBuffer[(uint8_t)isExternalRequest + 1];
    if ((floor == elevatorControl.currentFloorIndex) &&
        (elevatorControl.doorState != EDS_Closed))
    {
        elevatorOpenDoorCounter = ElevatorOpenDoorCounterDefault;
        elevatorCloseDoorCounter = ElevatorCloseDoorCounterDefault;
        elevatorDoorTextBlinkCounter = ElevatorDoorTextBlinkCounterDefault;
        elevatorDoorTextVisible = 1;
        elevatorControl.doorState = EDS_Open;
        NumberInput_clear();
        workState = WS_GetElevatorControl;
        return;
    } else if (numberInput.inputBuffer[0] == SK_Up)
        ElevatorControl_makeRequest(floor, FR_Up);
    else if (numberInput.inputBuffer[0] == SK_Down)
        ElevatorControl_makeRequest(floor, FR_Down);
    else
        ElevatorControl_makeRequest(floor, FR_Inside);
    NumberInput_clear();
    workState = WS_GetElevatorControl;
}

void updateElevatorStatus()
{
    char floorStr[2];
    updateAllFloorRequestDisplay();
    LcdDisplay_setCursorPos(3);
    if (elevatorControl.doorState == EDS_Closing)
        LcdDisplay_sendEmptyString(2);
    else
    {
        myItoa(
            ElevatorControl_indexToFloor(elevatorControl.currentFloorIndex),
            floorStr
        );
        LcdDisplay_sendString(floorStr, 2);
    }
    elevatorMoveCounter--;
    if (elevatorMoveCounter == 0)
    {
        elevatorMoveCounter = ElevatorMoveCounterDefault;
        ElevatorControl_move();
    }
    if (elevatorControl.doorState == EDS_Open)
    {
        Led_allOff();
        LcdDisplay_setCursorPos(0);
        if (elevatorDoorTextVisible)
            LcdDisplay_sendString(openDoorPrompt, 2);
        else
            LcdDisplay_sendEmptyString(2);
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
        LcdDisplay_setCursorPos(0);
        if (elevatorDoorTextVisible)
            LcdDisplay_sendString(closeDoorPrompt, 2);
        else
            LcdDisplay_sendEmptyString(2);
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
    } else if (elevatorControl.runState != ERS_Idle)
    {
        LcdDisplay_setCursorPos(0);
        if (elevatorControl.runState == ERS_MovingUp)
            LcdDisplay_sendData('^');
        else
            LcdDisplay_sendData('v');
    }
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
        int8_t floor = keyboard.releasedKey;
        myItoa(
            ElevatorControl_indexToFloor(floor),
            (char*)&display.displayBuffer[numberInput.currentIndex]
        );
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
