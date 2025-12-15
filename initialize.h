#ifndef __INITIALIZE_H__
#define __INITIALIZE_H__

void init();
#define initPin()                                                                                  \
    {                                                                                              \
        P0MDOUT = 0xFF;                                                                            \
        P1MDOUT = 0xFF;                                                                            \
        P2MDOUT = 0xFF;                                                                            \
        XBR2 = 0x40;                                                                               \
    }
#define initWatchdog()                                                                             \
    {                                                                                              \
        WDTCN = 0xDE;                                                                              \
        WDTCN = 0xAD;                                                                              \
    }
#define initTimer()                                                                                \
    {                                                                                              \
        CKCON = 0x08;                                                                              \
        TCON = 0x10;                                                                               \
        TMOD = 0x01;             /* Timer 0 16 bit */                                              \
        TH0 = (-20000) / 0x0100; /*10 ms in 12MHz*/                                                \
        TL0 = (-20000) % 0x0100;                                                                   \
        IE = 0x02;                                                                                 \
        EA = 1;                                                                                    \
    }

#endif // __INITIALIZE_H__
