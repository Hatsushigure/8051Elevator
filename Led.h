#ifndef __LED_H__
#define __LED_H__

#include "utils.h"

#define Led_init()                                                                                 \
    {                                                                                              \
        P3 = 0x00;                                                                                 \
    }

#define Led_allOn()                                                                                \
    {                                                                                              \
        P3 = 0xFF;                                                                                 \
    }

#define Led_allOff()                                                                               \
    {                                                                                              \
        P3 = 0x00;                                                                                 \
    }

#endif // __LED_H__
