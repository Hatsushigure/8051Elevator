#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "utils.h"

enum DisplaySegement
{
    DS_Top = 0xFE,
    DS_TopRight = 0xFD,
    DS_BottomRight = 0xFB,
    DS_Bottom = 0xF7,
    DS_BottomLeft = 0xEF,
    DS_TopLeft = 0xDF,
    DS_Middle = 0xBF,
    DS_Dot = 0x7F,
    DS_Disabled = 0xFF
};

enum DisplatCharacter
{
    DC_0 = DS_Top & DS_TopRight & DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_TopLeft,
    DC_1 = DS_TopRight & DS_BottomRight,
    DC_2 = DS_Top & DS_TopRight & DS_Middle & DS_BottomLeft & DS_Bottom,
    DC_3 = DS_Top & DS_TopRight & DS_Middle & DS_BottomRight & DS_Bottom,
    DC_4 = DS_TopRight & DS_BottomRight & DS_TopLeft & DS_Middle,
    DC_5 = DS_Top & DS_Middle & DS_BottomRight & DS_Bottom & DS_TopLeft,
    DC_6 = DS_Top & DS_Middle & DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_TopLeft,
    DC_7 = DS_Top & DS_TopRight & DS_BottomRight,
    DC_8 =
        DS_Top & DS_TopRight & DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_9 = DS_Top & DS_TopRight & DS_BottomRight & DS_Bottom & DS_Middle & DS_TopLeft,
    DC_A = DS_Top & DS_TopRight & DS_BottomRight & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_B = DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_C = DS_Top & DS_Bottom & DS_BottomLeft & DS_TopLeft,
    DC_D = DS_TopRight & DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_Middle,
    DC_E = DS_Top & DS_Bottom & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_F = DS_Top & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_H = DS_TopRight & DS_BottomRight & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_R = DS_BottomLeft & DS_Middle,
    DC_L = DS_Bottom & DS_BottomLeft & DS_TopLeft,
    DC_N = DS_TopRight & DS_BottomRight & DS_BottomLeft & DS_TopLeft & DS_Top,
    DC_O = DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_Middle,
    DC_U = DS_TopRight & DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_TopLeft
};

extern uint8_t displayBuffer[8];
extern uint8_t promptCount;
extern uint8_t currentPromptValue;
extern int8_t disappearCounter[8];

void shortDelay(uint8_t time);
void displayCharacter(uint8_t item, uint8_t index);
void displayString(uint8_t* str);
void refreshDisplay();
void promptInput(uint8_t index);
void delayDisappear(uint8_t index, int8_t time);
void resetDelayDisappear();

#endif // __DISPLAY_H__
