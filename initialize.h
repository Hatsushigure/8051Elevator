#ifndef __INITIALIZE_H__
#define __INITIALIZE_H__

#include "utils.h"

void init();
#define initPin()                                                              \
    {                                                                          \
        P0MDOUT = 0xFF;                                                        \
        P1MDOUT = 0xFF;                                                        \
        P2MDOUT = 0xFF;                                                        \
        P3MDOUT = 0xFF;                                                        \
        XBR2 = 0x40;                                                           \
    }
#define initWatchdog()                                                         \
    {                                                                          \
        WDTCN = 0xDE; /* Kill watchdog */                                      \
        WDTCN = 0xAD;                                                          \
    }
#define initTimer()                                                            \
    {                                                                          \
        CKCON = 0x18;                                                          \
        TCON = 0x50; /* Enable T0 and T1 */                                    \
        TMOD = 0x11; /* Timer 0 and 1 in mode 1 */                             \
        refillTimer0();                                                        \
        refillTimer1();                                                        \
        IE = 0x8A; /* Enable Timer 0 and 1 interrupt */                        \
    }

#endif // __INITIALIZE_H__
