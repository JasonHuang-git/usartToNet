#ifndef _USART_H
#define _USART_H

#include "pbdata.h"
#include "stdio.h"
#include "stdarg.h"

static void usartDataInit(void);

static void usartGPIOConfigure(void);

static void usartConfigure(void);

static void usartRccInit(void);

static void putStr(char *str);

static void nivcConfiguration(void);

static void dmaRccInit(void);

static void dmaConfiguration(void);

extern void sendData(void);

extern void putChar(char ch);

extern void usartInit(void);

extern void usartPrintf(char *fmt,...);

#endif
