#ifndef __INITIALIZE_H__
#define __INITIALIZE_H__

#include "utils.h"

void init();
void initPin();
#define initWatchdog()                                                         \
    {                                                                          \
        WDTCN = 0xDE; /* Kill watchdog */                                      \
        WDTCN = 0xAD;                                                          \
    }
void initTimer();

#endif // __INITIALIZE_H__
