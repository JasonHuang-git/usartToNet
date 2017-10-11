#ifndef _MODBUS_H
#define _MODBUS_H

#include "pbdata.h"

#define MaxData 64

typedef struct {
	u8 buf[MaxData];
	u8 count;
	int flag;
	int slavAddr;	
}ModBusData;

extern ModBusData sendData;
extern ModBusData recvData;

static u8 checkAddrIsMatch(const u8 *buf, const u8 count);

static void modbusSendError(u8 errCode, u8 type);

static void modbusParseFunctionCode03(const u8 *buf);	

static void modbusParseFunctionCode02(const u8 *buf);

static void modbusParseFunctionCode16(const u8 *buf);

static void modbusDealMaster(void);

extern void modbusInit(void);

extern void modbus(void);

extern void sendFunctionCode16(u16 startAddr, u16 count);

extern void sendFunctionCode03(u16 startAddr, u16 count);

extern void sendFunctionCode02(u16 startAddr, u16 count);

//在Pbdata中定义；
extern void modbusHandFunctionCode02(const u8 *buf);
extern void modbusHandFunctionCode03(const u16 *buf);
extern void getFunctionCode16Data(u16 *buf);
extern void executeTask(const u8 cmd);

#endif
