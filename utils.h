#ifndef __UTILS_H__
#define __UTILS_H__

#include <c8051F020.h>

#define shortDelay(time) while (time--);

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;

#endif // __UTILS_H__