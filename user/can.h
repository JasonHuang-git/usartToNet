#ifndef _CAN_H
#define _CAN_H

#include "pbdata.h"

static void canRCCInit(void);
static void canGPIOConfigure(void);
static void canConfigure(void);
static void canNvicConfigure(void);

extern void canInit(void);
extern void canWrite(const char *p);

#endif
