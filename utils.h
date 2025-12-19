#ifndef __UTILS_H__
#define __UTILS_H__

#include <SiLABS/c8051F020.h>

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;

#define shortDelay(time)                                                       \
    while (time--)                                                             \
        ;

#define refillTimer0()                                                         \
    {                                                                          \
        TH0 = (uint16_t)(-20000) >> 8;     /*10 ms in 2MHz*/                   \
        TL0 = (uint16_t)(-20000) & 0x00FF; /*10 ms in 2MHz*/                   \
    }

#define refillTimer1()                                                         \
    {                                                                          \
        TH1 = (uint16_t)(-40000) >> 8;     /*20 ms in 2MHz*/                   \
        TL1 = (uint16_t)(-40000) & 0x00FF; /*20 ms in 2MHz*/                   \
    }

void myItoa(int8_t value, char* str);

#endif // __UTILS_H__
