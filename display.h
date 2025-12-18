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

enum DisplayCharacter
{
    DC_0 = DS_Top & DS_TopRight & DS_BottomRight & DS_Bottom & DS_BottomLeft &
           DS_TopLeft,
    DC_1 = DS_TopRight & DS_BottomRight,
    DC_2 = DS_Top & DS_TopRight & DS_Middle & DS_BottomLeft & DS_Bottom,
    DC_3 = DS_Top & DS_TopRight & DS_Middle & DS_BottomRight & DS_Bottom,
    DC_4 = DS_TopRight & DS_BottomRight & DS_TopLeft & DS_Middle,
    DC_5 = DS_Top & DS_Middle & DS_BottomRight & DS_Bottom & DS_TopLeft,
    DC_6 = DS_Top & DS_Middle & DS_BottomRight & DS_Bottom & DS_BottomLeft &
           DS_TopLeft,
    DC_7 = DS_Top & DS_TopRight & DS_BottomRight,
    DC_8 = DS_Top & DS_TopRight & DS_BottomRight & DS_Bottom & DS_BottomLeft &
           DS_TopLeft & DS_Middle,
    DC_9 = DS_Top & DS_TopRight & DS_BottomRight & DS_Bottom & DS_Middle &
           DS_TopLeft,
    DC_A = DS_Top & DS_TopRight & DS_BottomRight & DS_BottomLeft & DS_TopLeft &
           DS_Middle,
    DC_B = DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_C = DS_Top & DS_Bottom & DS_BottomLeft & DS_TopLeft,
    DC_D = DS_TopRight & DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_Middle,
    DC_E = DS_Top & DS_Bottom & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_F = DS_Top & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_H =
        DS_TopRight & DS_BottomRight & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_R = DS_BottomLeft & DS_Middle,
    DC_L = DS_Bottom & DS_BottomLeft & DS_TopLeft,
    DC_N = DS_TopRight & DS_BottomRight & DS_BottomLeft & DS_TopLeft & DS_Top,
    DC_O = DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_Middle,
    DC_P = DS_Top & DS_TopRight & DS_BottomLeft & DS_TopLeft & DS_Middle,
    DC_U = DS_TopRight & DS_BottomRight & DS_Bottom & DS_BottomLeft & DS_TopLeft
};

typedef struct
{
    uint8_t displayBuffer[6];
    uint8_t promptCount;
    uint8_t currentPromptValue;
    int8_t disappearCounter[6];
} Display;

extern Display display;

void Display_init();
void Display_displayCharacter(uint8_t item, uint8_t index);
void Display_refreshDisplay();
void Display_promptInput(uint8_t index);
#define Display_delayDisappear(index, time)                                    \
    display.disappearCounter[index] = time;
void Display_resetDelayDisappear();
void Display_clear();
void Display_setPrompt(uint8_t* prompt, uint8_t size);

#endif // __DISPLAY_H__
