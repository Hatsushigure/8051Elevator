#ifndef __LCDDISPLAY_H__
#define __LCDDISPLAY_H__

#include "utils.h"

typedef enum
{
    CMD_ClearScreen = 0x01,
    CMD_CursorHome = 0x02,
    CMD_InputModeSet = 0x04,
    CMD_InputModeSet_CursorDecrement = 0x00,
    CMD_InputModeSet_CursorIncrement = 0x02,
    CMD_InputModeSet_NoShift = 0x00,
    CMD_InputModeSet_Shift = 0x01,
    CMD_DisplaySwitch = 0x08,
    CMD_DisplaySwitch_DisplayOff = 0x00,
    CMD_DisplaySwitch_DisplayOn = 0x04,
    CMD_DisplaySwitch_CursorOff = 0x00,
    CMD_DisplaySwitch_CursorOn = 0x02,
    CMD_DisplaySwitch_BlinkOff = 0x00,
    CMD_DisplaySwitch_BlinkOn = 0x01,
    CMD_Shift = 0x10,
    CMD_Shift_Cursor = 0x00,
    CMD_Shift_Display = 0x08,
    CMD_Shift_Left = 0x00,
    CMD_Shift_Right = 0x04,
    CMD_FunctionSet = 0x20,
    CMD_FunctionSet_4Bit = 0x00,
    CMD_FunctionSet_8Bit = 0x10,
    CMD_FunctionSet_1Line = 0x00,
    CMD_FunctionSet_2Line = 0x08,
    CMD_FunctionSet_5x8Dots = 0x00,
    CMD_FunctionSet_5x10Dots = 0x04,
    CMD_SetCGRAMAddr = 0x40,
    CMD_SetDDRAMAddr = 0x80,
} LcdCommand;

void LcdDisplay_init();
void LcdDisplay_sendCommand(uint8_t command);
void LcdDisplay_sendData(char payload);
void LcdDisplay_sendString(const char* str, uint8_t length);
#define LcdDisplay_clear()                                                     \
    {                                                                          \
        uint16_t dly = 1000;                                                   \
        LcdDisplay_sendCommand(CMD_ClearScreen);                               \
        shortDelay(dly); /* Clear screen needs a longer delay */               \
    }
#define LcdDisplay_setCursorPos(idx)                                           \
    LcdDisplay_sendCommand(CMD_SetDDRAMAddr | (idx))
#define LcdDisplay_fastClearLine(line)                                         \
    {                                                                          \
        uint8_t i = 16;                                                        \
        LcdDisplay_setCursorPos(line * 0x40);                                  \
        LcdDisplay_sendCommand(CMD_SetDDRAMAddr);                              \
        while (i--)                                                            \
            LcdDisplay_sendData(' ');                                         \
    }

#endif // __LCDDISPLAY_H__
