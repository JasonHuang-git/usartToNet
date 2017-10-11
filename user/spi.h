#ifndef _SPI_H
#define _SPI_H

#include "pbdata.h"

static void spiGPIOInit(void);
static void spiConfigure(void);
static void spiRccinit(void);
static void spiNivcConfiguration(void);
static void initData(void);
extern void spiInit(void);
extern void spiWrite(const char *p);

static void spi2GPIOInit(void);
static void spi2Configure(void);
static void spi2NivcConfiguration(void);

#endif

