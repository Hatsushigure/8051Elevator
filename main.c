#include "Joystick.h"
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

enum WorkState
{
    WS_GetPassword,
    WS_VarifyPassword,
    WS_PasswordWrong,
    WS_GetMaxPerson,
    WS_GetMaxWeight,
    WS_Finish,
    WS_GetElevatorControl,
    WS_Free
};

enum ElevatorRunState
{
    ERS_Idle,
    ERS_MovingUp,
    ERS_MovingDown
};

const uint8_t code castTable[] = {DC_0,        DC_1, DC_2,        DC_3,
                                  DC_4,        DC_5, DC_6,        DC_7,
                                  DC_8,        DC_9, DS_Middle,   DC_B,
                                  DC_C,        DC_D, DC_E,        DC_F,
                                  DS_Disabled, DC_D, DS_Disabled, DC_U};

// Prompts and Password
const uint8_t code correctPassword[] = {1, 1, 4, 5, 1, 4};
const uint8_t code personPrompt[] = {DC_N, DC_O& DS_Dot};
const uint8_t code weightPrompt[] = {DC_R, DC_L& DS_Dot};
const uint8_t code finishPrompt[] = {DC_F, DC_1, DC_N, DC_1, DC_5, DC_H};
const uint8_t code errorPrompt[] = {DC_E, DC_R, DC_R};

uint8_t workState;
uint8_t passwordTrialCount = 5;
uint8_t wrongPasswordDelayTime;
uint8_t finishDelayTime = 80;
uint8_t maxPerson;
uint8_t maxWeight;
bit upDownState; // 0 for up, 1 for down

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
        getContent(3, 0, 0, 0, WS_Free, 0, 1);                                 \
    }
void varifyPassword();
void wrongPasswordDelay();
void finishDelay();

int main()
{
    init();
    workState = WS_GetPassword;
    while (1)
        ;
}

void onTimer0Timeout() INTERRUPT(1)
{
    EA = 0;
    TH0 = (-20000) / 0x0100; // 10 ms in 2MHz
    TL0 = (-20000) % 0x0100;
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
    case WS_Free:
        break;
    }
    Display_refreshDisplay();
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
        workState = WS_GetElevatorControl;
    }
    finishDelayTime--;
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
        display.displayBuffer[promptLength + numberInput.currentIndex] =
            DS_Disabled;
        NumberInput_backspace();
        return;
    }
    if (numberInput.currentIndex == contentLength)
        return;
    if (keyboard.releasedKey > (9 + (uint8_t)isElevatorControl))
        return;
    display.displayBuffer[promptLength + numberInput.currentIndex] =
        castTable[keyboard.releasedKey];
    if (isPassword)
        Display_delayDisappear(promptLength + numberInput.currentIndex, 50);
    NumberInput_append(keyboard.releasedKey);
}
