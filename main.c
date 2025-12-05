#include "MaxPersonInput.h"
#include "PasswordInput.h"
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
    WS_Person,
    WS_Weight
};
uint8_t workState = WS_GetPassword;
uint8_t castTable[] = {
    DC_0, DC_1, DC_2, DC_3, DC_4, DC_5, DC_6, DC_7, DC_8, DC_9, DC_A, DC_B, DC_C, DC_D, DC_E, DC_F
};
uint8_t truePassword[] = {1, 1, 4, 5, 1, 4};
uint8_t personPrompt[] = {
    DC_N, DC_O& DS_Dot, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled
};
uint8_t errorPrompt[] = {DC_E, DC_R, DC_R, DC_O, DC_R, DS_Disabled, DS_Disabled, DS_Disabled};
uint8_t weightPrompt[] = {
    DC_R, DC_L& DS_Dot, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled, DS_Disabled
};

void getPassword();
void getMaxPerson();
void varifyPassword();

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
    case WS_Person:
        getMaxPerson();
        break;
    case WS_Weight:
        break;
    }
    refreshDisplay();
    EA = 1;
}

void getPassword()
{
    promptInput(passwordInput.currentIndex);
    Keyboard_getKey(&keyboard);
    if (keyboard.state != Released)
        return;
    if (keyboard.releasedKey == SK_Enter)
    {
        workState = WS_VarifyPassword;
        return;
    }
    if (keyboard.releasedKey == SK_Backspace)
    {
        displayBuffer[passwordInput.currentIndex] = DS_Disabled;
        PasswordInput_backspace(&passwordInput);
        return;
    }
    if (passwordInput.currentIndex == 6)
        return;
    if (keyboard.releasedKey >= 10)
        return;
    displayBuffer[passwordInput.currentIndex] = castTable[keyboard.releasedKey];
    delayDisappear(passwordInput.currentIndex, 50);
    PasswordInput_append(&passwordInput, keyboard.releasedKey);
}

void getMaxPerson()
{
    uint8_t i = 0;
    promptInput(2 + maxPersonInput.currentIndex);
    Keyboard_getKey(&keyboard);
    if (keyboard.state != Released)
        return;
    if (keyboard.releasedKey == SK_Enter)
    {
        workState = WS_Weight;
        for (; i < 8; i++)
            displayBuffer[i] = weightPrompt[i];
        return;
    }
    if (keyboard.releasedKey == SK_Backspace)
    {
        displayBuffer[2 + maxPersonInput.currentIndex] = DS_Disabled;
        MaxPersonInput_backspace(&maxPersonInput);
        return;
    }
    if (maxPersonInput.currentIndex == 3)
        return;
    if (keyboard.releasedKey >= 10)
        return;
    displayBuffer[2 + maxPersonInput.currentIndex] = castTable[keyboard.releasedKey];
    MaxPersonInput_append(&maxPersonInput, keyboard.releasedKey);
}

void varifyPassword()
{
    uint8_t i = 0;
    bit passwordIsCorrect = 1;
    uint8_t* targetPrompt;
    for (; i < 6; i++)
    {
        if (passwordInput.passwordInput[i] != truePassword[i])
        {
            passwordIsCorrect = 0;
            break;
        }
    }
    if (!passwordIsCorrect)
        targetPrompt = errorPrompt;
    else
    {
        workState = WS_Person;
        targetPrompt = personPrompt;
    }
    resetDelayDisappear();
    for (i = 0; i < 8; i++)
        displayBuffer[i] = targetPrompt[i];
}
