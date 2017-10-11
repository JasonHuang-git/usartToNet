#ifndef _LED_H
#define _LED_H

#include "pbdata.h"

static void ledRccInit(void);

extern void ledInit(void);

extern void ledOn(void);

extern void ledOff(void);

extern LedStatus ledStatus(void);

#endif
