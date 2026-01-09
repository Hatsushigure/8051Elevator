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
    WS_GetDoConfigDisabledFloor,
    WS_GetDisabledFloor,
    WS_Finish,
    WS_GetElevatorControl,
    WS_ProcessElevatorControl,
} WorkState;

const uint8_t code castTable[] = {DC_0,        DC_1, DC_2,        DC_3,
                                  DC_4,        DC_5, DC_6,        DC_7,
                                  DC_8,        DC_9, DS_Middle,   DC_B,
                                  DC_C,        DC_D, DC_E,        DC_F,
                                  DS_Disabled, DC_D, DS_Disabled, DC_U};

WorkState workState;
uint8_t passwordTrialCount;
uint8_t wrongPasswordDelayTime;
uint8_t finishDelayTime;
uint8_t maxPerson;
uint8_t maxWeight;
bit configurationComplete;
uint8_t elevatorDoorTextBlinkCounter =
    ElevatorDoorTextBlinkCounterDefault; // Used to set a 0.5 s blink
uint8_t elevatorOpenDoorCounter =
    ElevatorOpenDoorCounterDefault;      // Used to keep door open for 5s
uint8_t elevatorCloseDoorCounter =
    ElevatorCloseDoorCounterDefault;     // Used to keep door closing for 2s
uint8_t elevatorMoveCounter =
    ElevatorMoveCounterDefault; // Elevator state update when it reach 0
bit elevatorDoorTextVisible = 1;
bit promptIsSet = 0;
bit doGetDisableedFloor = 0;
uint8_t updateElevatorStatusCounter;
uint8_t keyboardPreviousReleasedKey = 0xFF;
uint8_t doubleClickTimeCounter = DoubleClickTimeCounterInitial;
uint16_t specialSequenceTimeLimit = SpecialSequenceTimeLimitInitial;
uint8_t specialKeySequenceCurrentIndex;

void getContentEx(
    const char* prompt,
    uint8_t promptLength,
    uint8_t contentLength,
    uint8_t nextState,
    bit isPassword,
    bit isElevatorFloor,
    bit isUint8
);
void getPassword();
void getMaxPerson();
void getMaxWeight();
void getDoConfigDisabledFloor();
void getDisabledFloor();
void getElevatorControl();
void varifyPassword();
void wrongPasswordDelay();
void finishDelay();
void setupAllFloorRequestDisplay();
void updateAllFloorRequestDisplay();
void processElevatorControl();
void updateElevatorStatus();
void enterConfiguration();

int main()
{
    init();
    enterConfiguration();
    while (1)
    {
        switch (workState) // Main state machine
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
        case WS_GetDoConfigDisabledFloor:
            getDoConfigDisabledFloor();
            break;
        case WS_GetDisabledFloor:
            getDisabledFloor();
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
        }
        if (!configurationComplete)
            continue;
        if (!updateElevatorStatusCounter)
        {
            updateElevatorStatusCounter = UpdateElevatorStatusCounterInitial;
            updateElevatorStatus();
        }
    }
}

void onTimer0Timeout() INTERRUPT(1)
{
    EA = 0;
    refillTimer0();
    Keyboard_getKey();
    Joystick_getKey();
    Display_refreshDisplay();
    wrongPasswordDelayTime--;
    finishDelayTime--;
    updateElevatorStatusCounter--;
    doubleClickTimeCounter--;
    if (doubleClickTimeCounter == 0xFF)
        doubleClickTimeCounter = 0;
    specialSequenceTimeLimit--;
    if (specialSequenceTimeLimit == 0xFFFF)
        specialSequenceTimeLimit = 0;
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
            if (numberInput.inputBuffer[i] != CorrectPassword[i])
            {
                passwordIsCorrect = 0;
                break;
            }
        }
    }
    NumberInput_clear();
    if (!passwordIsCorrect)
    {
        LcdDisplay_println(PasswordWrongPrompt, 14, 0);
        LcdDisplay_setCursorPos(0x40);
        LcdDisplay_sendString(TrialCountPrompt, 11);
        LcdDisplay_sendData(' ');
        LcdDisplay_sendData('0' + passwordTrialCount);
        LcdDisplay_sendEmptyString(2);
        wrongPasswordDelayTime = 100;
        workState = WS_PasswordWrong;
    } else
        workState = WS_GetMaxPerson;
}

void wrongPasswordDelay()
{
    wrongPasswordDelayTime = WrongPasswordDelayTimeInitial;
    while (!passwordTrialCount)
        ;
    while (wrongPasswordDelayTime)
        ;
    workState = WS_GetPassword;
}

void finishDelay()
{
    finishDelayTime = FinishDelayTimeInitial;
    LcdDisplay_disableCursor();
    LcdDisplay_println(FinishPrompt, 13, 0);
    LcdDisplay_clearLine(1);
    while (finishDelayTime)
        ;
    Led_allOn();
    LcdDisplay_clearLine(0);
    setupAllFloorRequestDisplay();
    workState = WS_GetElevatorControl;
    configurationComplete = 1;
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
    uint8_t i = 10;
    while (i--)
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
    elevatorMoveCounter--;
    if (elevatorMoveCounter == 0)
    {
        elevatorMoveCounter = ElevatorMoveCounterDefault;
        ElevatorControl_move();
    }
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
    if (elevatorControl.doorState == EDS_Open)
    {
        Led_allOff();
        LcdDisplay_setCursorPos(0);
        if (elevatorDoorTextVisible)
            LcdDisplay_sendString(OpenDoorPrompt, 2);
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
            LcdDisplay_sendString(CloseDoorPrompt, 2);
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

void enterConfiguration()
{
    configurationComplete = 0;
    specialKeySequenceCurrentIndex = 0;
    workState = WS_GetPassword;
    Led_allOff();
    NumberInput_clear();
    passwordTrialCount = MaxPasswordTrialCount;
    Display_clear();
}

void getContentEx(
    const char* prompt,
    uint8_t promptLength,
    uint8_t contentLength,
    uint8_t nextState,
    bit isPassword,
    bit isElevatorFloor,
    bit isUint8
)
{
    if (!promptIsSet)
    {
        LcdDisplay_println(prompt, promptLength, 0);
        LcdDisplay_clearLine(1);
        LcdDisplay_setCursorPos(0x40);
        LcdDisplay_enableCursor();
        promptIsSet = 1;
    }
    if (keyboard.state != KS_Released)
        return;
    keyboard.state = KS_Free;
    if (keyboard.releasedKey == SK_Enter)
    {
        if (numberInput.currentIndex == 0)
            return;
        NumberInput_getNumber();
        if (isUint8 && (numberInput.result > 255 || numberInput.result == 0))
            return;
        if (!isPassword)
            NumberInput_clear();
        promptIsSet = 0;
        workState = nextState;
        return;
    }
    if (keyboard.releasedKey == SK_Backspace)
    {
        if (!numberInput.currentIndex)
            return;
        LcdDisplay_backspace();
        if (isElevatorFloor)
            LcdDisplay_backspace();
        NumberInput_backspace();
        return;
    }
    if (numberInput.currentIndex >= contentLength)
        return;
    if (keyboard.releasedKey > 9)
        return;
    if (isPassword)
        LcdDisplay_sendData('*');
    else if (isElevatorFloor)
    {
        myItoa(
            ElevatorControl_indexToFloor(keyboard.releasedKey),
            numberInput.inputBuffer + 1
        );
        LcdDisplay_sendString(numberInput.inputBuffer + 1, 2);
    } else
        LcdDisplay_sendData(keyboard.releasedKey + '0');
    NumberInput_append(keyboard.releasedKey);
}

void getPassword()
{
    getContentEx(PasswordPrompt, 9, 6, WS_VarifyPassword, 1, 0, 0);
}

void getMaxPerson()
{
    getContentEx(PersonPrompt, 11, 3, WS_GetMaxWeight, 0, 0, 1);
    maxWeight = numberInput.result;
}

void getMaxWeight()
{
    getContentEx(WeightPrompt, 11, 3, WS_GetDoConfigDisabledFloor, 0, 0, 1);
    maxPerson = numberInput.result;
}

void getDoConfigDisabledFloor()
{
    getContentEx(
        AskConfigDisableFloorPrompt, 16, 1, WS_GetDisabledFloor, 0, 0, 0
    );
    doGetDisableedFloor = (numberInput.result == 1);
}

void getDisabledFloor()
{
    if (!doGetDisableedFloor)
    {
        workState = WS_Finish;
        return;
    }
    getContentEx(
        DisableFloorPrompt, 13, 1, WS_GetDoConfigDisabledFloor, 0, 1, 0
    );
    elevatorControl.floorEnableStatus[numberInput.result] = 0;
}

void getElevatorControl()
{
    int8_t floor;
    uint8_t currentReleasedKey;
    Display_promptInput(numberInput.currentIndex);

    // Store keys and reset keyboard and joystick
    if (joystick.releasedKey != 0xFF)
        currentReleasedKey = joystick.releasedKey;
    else if (keyboard.state == KS_Released)
        currentReleasedKey = keyboard.releasedKey;
    else
        return;
    keyboard.state = KS_Free;
    joystick.releasedKey = 0xFF;

    // Process special sequence
    if (!specialKeySequenceCurrentIndex)
        specialSequenceTimeLimit = SpecialSequenceTimeLimitInitial;
    if (specialSequenceTimeLimit &&
        currentReleasedKey ==
            SpecialKeySequence[specialKeySequenceCurrentIndex])
        specialKeySequenceCurrentIndex++;
    else
        specialKeySequenceCurrentIndex = 0;
    if (specialKeySequenceCurrentIndex == 12)
    {
        enterConfiguration();
        return;
    }

    // Process prefix input
    if (!numberInput.currentIndex)
    {
        if (currentReleasedKey == SK_Up || currentReleasedKey == SK_Down)
        {
            NumberInput_append(currentReleasedKey);
            display.displayBuffer[0] = castTable[currentReleasedKey];
        }
    }

    // Process double click
    if (doubleClickTimeCounter && currentReleasedKey < SK_A &&
        currentReleasedKey == keyboardPreviousReleasedKey)
    {
        keyboardPreviousReleasedKey = 0xFF;
        Display_clear();
        NumberInput_clear();
        ElevatorControl_cancleInternalRequest(keyboard.releasedKey);
        return;
    }
    keyboardPreviousReleasedKey = currentReleasedKey;
    doubleClickTimeCounter = DoubleClickTimeCounterInitial;

    if (currentReleasedKey == SK_Enter)
    {
        Display_clear();
        workState = WS_ProcessElevatorControl;
        return;
    }
    if (currentReleasedKey == SK_Backspace)
    {
        display.displayBuffer[numberInput.currentIndex] = DS_Disabled;
        NumberInput_backspace();
        display.displayBuffer[numberInput.currentIndex] = DS_Disabled;
        NumberInput_backspace();
        return;
    }
    if (numberInput.currentIndex >= 2)
        return;
    if (currentReleasedKey > 9)
        return;
    floor = currentReleasedKey;
    myItoa(
        ElevatorControl_indexToFloor(floor),
        (char*)&display.displayBuffer[numberInput.currentIndex]
    );
    if (display.displayBuffer[numberInput.currentIndex] == '-')
        display.displayBuffer[numberInput.currentIndex] = DS_Middle;
    else
        display.displayBuffer[numberInput.currentIndex] = DS_Disabled;
    display.displayBuffer[numberInput.currentIndex + 1] =
        castTable[display.displayBuffer[numberInput.currentIndex + 1] - '0'];
    NumberInput_append(0);
    NumberInput_append(floor);
    return;
}
