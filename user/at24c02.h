#ifndef _AT24C02_H
#define _AT24C02_H

#include "pbdata.h"

extern u8 AT24Cxx_ReadOneByte(u16 addr);
extern u16 AT24Cxx_ReadTwoByte(u16 addr);
extern void AT24Cxx_WriteOneByte(u16 addr,u8 dt);
extern void AT24Cxx_WriteTwoByte(u16 addr,u16 dt);

#endif
