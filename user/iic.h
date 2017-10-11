#ifndef _IIC_H
#define _IIC_H

#include "pbdata.h"

#define ClockSpeedKHz(a) (a) * 1000
#define SlaveDevice 	0xA0

static void iicRccInit(void);
static void iicGPIOInit(void);
static void iicConfigure(void);
static void iicNvicConfigure(void);

extern void writeData(char *data, int len, int addr);
extern void iicInit(void);

#endif
