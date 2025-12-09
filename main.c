#include "NumberInput.h"
#include "display.h"
#include "initialize.h"
#include "keyboard.h"
#include "utils.h"
#include <reg51.h>

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
    WS_Free
};

const uint8_t code castTable[] = {
    DC_0, DC_1, DC_2, DC_3, DC_4, DC_5, DC_6, DC_7, DC_8, DC_9, DC_A, DC_B, DC_C, DC_D, DC_E, DC_F
};

// Prompts and Password
const uint8_t code truePassword[] = {1, 1, 4, 5, 1, 4};
const uint8_t code personPrompt[] = {DC_N, DC_O& DS_Dot};
const uint8_t code weightPrompt[] = {DC_R, DC_L& DS_Dot};
const uint8_t code finishPrompt[] = {DC_F, DC_1, DC_N, DC_1, DC_5, DC_H};
const uint8_t code lockedPrompt[] = {DC_L, DC_O, DC_C, DC_A, DC_E, DC_D, DS_Disabled, DS_Disabled};
const uint8_t code errorPrompt[] = {DC_E, DC_R, DC_R, DC_O, DC_R};

// State machine variables
uint8_t workState;
uint8_t passwordTrialCount = 5;
uint8_t wrongPasswordDelayTime;
uint8_t finishDelayTime = 80;
uint8_t maxPerson;
uint8_t maxWeight;
uint8_t workState;

void getContent(
    uint8_t contentLength,
    uint8_t promptLength,
    const uint8_t code* nextPrompt,
    uint8_t nextPromptSize,
    uint8_t nextState,
    bit isPassword
);
#define getPassword()                                                                              \
    {                                                                                              \
        getContent(6, 0, 0, 0, WS_VarifyPassword, 1);                                              \
    }
#define getMaxPerson()                                                                             \
    {                                                                                              \
        getContent(3, 2, weightPrompt, 2, WS_GetMaxWeight, 0);                                     \
        maxWeight = numberInput.result;                                                            \
    }
#define getMaxWeight()                                                                             \
    {                                                                                              \
        getContent(3, 2, finishPrompt, 6, WS_Finish, 0);                                           \
        maxPerson = numberInput.result;                                                            \
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
    TH0 = (-10000) / 0x0100; // 10 ms in 12MHz
    TL0 = (-10000) % 0x0100;
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
    case WS_Free:
        break;
    }
    Display_refreshDisplay(&display);
    EA = 1;
}

void varifyPassword()
{
    uint8_t i = 0;
    bit passwordIsCorrect = 1;
    passwordTrialCount--;
    if (numberInput.currentIndex != 6)
        passwordIsCorrect = 0;
    else
    {
        for (; i != 6; i++)
        {
            if (numberInput.inputBuffer[i] != truePassword[i])
            {
                passwordIsCorrect = 0;
                break;
            }
        }
    }
    if (!passwordIsCorrect)
    {
        Display_setPrompt(&display, errorPrompt, 8);
        display.displayBuffer[7] = castTable[passwordTrialCount];
        wrongPasswordDelayTime = 100;
        workState = WS_PasswordWrong;
    } else
    {
        Display_setPrompt(&display, personPrompt, 2);
        NumberInput_clear(&numberInput);
        workState = WS_GetMaxPerson;
    }
    Display_resetDelayDisappear(&display);
}

void wrongPasswordDelay()
{
    if (passwordTrialCount == 0)
        return;
    if (wrongPasswordDelayTime == 0)
    {
        Display_clear(&display);
        NumberInput_clear(&numberInput);
        workState = WS_GetPassword;
        return;
    }
    wrongPasswordDelayTime--;
}

void finishDelay()
{
    if (finishDelayTime == 0)
    {
        // Finish delay over, clear screen and enter idle state
        Display_clear(&display);
        workState = WS_Free;
    }
    finishDelayTime--;
}

void getContent(
    uint8_t contentLength,
    uint8_t promptLength,
    const uint8_t code* nextPrompt,
    uint8_t nextPromptSize,
    uint8_t nextState,
    bit isPassword
)
{
    Display_promptInput(&display, promptLength + numberInput.currentIndex);
    Keyboard_getKey(&keyboard);
    if (keyboard.state != Released)
        return;
    if (keyboard.releasedKey == SK_Enter)
    {
        numberInput.result = NumberInput_getNumber(&numberInput);
        if (!isPassword)
            NumberInput_clear(&numberInput);
        workState = nextState;
        Display_setPrompt(&display, nextPrompt, nextPromptSize);
        return;
    }
    if (keyboard.releasedKey == SK_Backspace)
    {
        display.displayBuffer[promptLength + numberInput.currentIndex] = DS_Disabled;
        NumberInput_backspace(&numberInput);
        return;
    }
    if (numberInput.currentIndex == contentLength)
        return;
    if (keyboard.releasedKey >= 10)
        return;
    display.displayBuffer[promptLength + numberInput.currentIndex] =
        castTable[keyboard.releasedKey];
    if (isPassword)
        Display_delayDisappear(&display, promptLength + numberInput.currentIndex, 50);
    NumberInput_append(&numberInput, keyboard.releasedKey);
}
