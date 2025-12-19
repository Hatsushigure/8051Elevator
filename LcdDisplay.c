#include "LcdDisplay.h"

#define LcdDataPort P5
#define LcdRsPin P4 & 0x20
#define LcdRwPin P4 & 0x40
#define LcdEnPin P4 & 0x80

#define setRsPin() P4 |= 0x20
#define clearRsPin() P4 &= ~0x20
#define setRwPin() P4 |= 0x40
#define clearRwPin() P4 &= ~0x40
#define setEnPin() P4 |= 0x80
#define clearEnPin() P4 &= ~0x80

void LcdDisplay_init()
{
    LcdDisplay_clear();
    LcdDisplay_sendCommand(
        CMD_FunctionSet | CMD_FunctionSet_8Bit | CMD_FunctionSet_2Line |
        CMD_FunctionSet_5x10Dots
    );
    LcdDisplay_sendCommand(
        CMD_DisplaySwitch | CMD_DisplaySwitch_DisplayOn |
        CMD_DisplaySwitch_CursorOff | CMD_DisplaySwitch_BlinkOff
    );
    LcdDisplay_sendCommand(
        CMD_InputModeSet | CMD_InputModeSet_CursorIncrement |
        CMD_InputModeSet_NoShift
    );
}

void LcdDisplay_sendCommand(uint8_t command)
{
    uint8_t dly = 10;
    shortDelay(dly);
    LcdDataPort = command;
    clearRsPin();
    clearRwPin();
    setEnPin();
    dly = 10;
    shortDelay(dly);
    clearEnPin();
}

void LcdDisplay_sendData(char payload)
{
    uint8_t dly = 10;
    shortDelay(dly);
    LcdDataPort = payload;
    setRsPin();
    clearRwPin();
    setEnPin();
    dly = 10;
    shortDelay(dly);
    clearEnPin();
}

void LcdDisplay_sendString(const char* str, uint8_t length)
{
    while (length--)
        LcdDisplay_sendData(*str++);
}
