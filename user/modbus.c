#include "modbus.h"

ModBusData recvData;
ModBusData sendData;

extern u16 timeout;

static u8 cmd = 0x03;

uint16_t crc16(const u8 *puchmsg, uint16_t len)
{
	uint16_t crc = 0xffff;
	uint16_t i,j;
	for(i = 0; i < len; i++){
		crc ^= puchmsg[i];
		for(j = 0; j < 8; j++){
			if(crc & 1){
				crc >>= 1;
				crc ^= 0xA001;
			}
			else {
				crc >>= 1;
			}
		}
	}
	return crc;
}

void clear(void)
{
	uint16_t i;
	for(i = 0; i < MaxData; i++){
		recvData.buf[i] = 0;
		sendData.buf[i] = 0;
	}
	recvData.count = 0;
	sendData.count = 0;
	recvData.slavAddr = 1;
	sendData.slavAddr = 1;
}

void modbusInit(void)
{
	sendData.flag = StartSend;
	sendData.flag = StartRecv;
	
	task = sendFunctionCode03;
}

void modbus(void)
{
	if(sendData.flag == FinishSend){
		recvData.flag = StartRecv;
		enableRecv();
	}
	else if(sendData.flag == StartSend){		
		enableSend();
		timeout = 0;
		
		executeTask(cmd);
	}
	if(recvData.flag == FinishRecv){
		modbusDealMaster();
		clear();
		
		sendData.flag = StartSend;
	}
}

void modbusDealMaster(void)
{
	if(checkAddrIsMatch(recvData.buf, recvData.count)){
		cmd = recvData.buf[1];
		switch(recvData.buf[1]){
			case 0x01: break;
			case 0x02: modbusParseFunctionCode02(recvData.buf);break;
			case 0x03: modbusParseFunctionCode03(recvData.buf);break;
			case 0x04: break;
			case 0x06: break;
			case 0x10: modbusParseFunctionCode16(recvData.buf);break;
			default: modbusSendError(recvData.buf[1],0x04);break;
		}
	}	
	else {
		modbusSendError(recvData.buf[1], 0x08);
	}		
}

u8 checkAddrIsMatch(const u8 *buf, const u8 count)
{
	u16 temp;
	if(count < 3){
		return false;
	}
	if(sendData.slavAddr != buf[0]){
		return false;
	}
	temp = buf[count - 1];
	temp <<= 8;
	temp |= buf[count - 2];
	
	return (temp == crc16(buf, count - 2));
}

/* 功能：返回错误的功能码
 * errCode：错误的功能码	
 * type：返回错误代码（
 *       08：CRC校验错误
 *		 01：功能码错误
 *		 02：地址错误
 *		 03：数据错误
 *		 04：不支持的功能码
 *
 */
void modbusSendError(u8 errCode, u8 type)
{
	u16  temp;
	sendData.buf[0] = sendData.slavAddr;
	sendData.buf[1] = 0x80 + errCode;
	sendData.buf[2] = type;
	temp = crc16(sendData.buf, 3);
	sendData.buf[4] = (u8)(temp >> 8);
	sendData.buf[3] = (u8)temp;
	sendData.count = 5;
}

void modbusParseFunctionCode03(const u8 *buf){
	u8 i;
	u16 val;
	u16 data[24];
	for(i = 0; i < buf[5]; ++i){
		val = buf[3 + 2 * i] << 8;
		val |= buf[4 + 2 * i];
		data[i] = val;
	}
	modbusHandFunctionCode03(data);
	
	task = sendFunctionCode16;
}

void modbusParseFunctionCode16(const u8 *buf){
	if(sendData.buf[3] != buf[3] || 
	   sendData.buf[5] != buf[5]){
		task = sendFunctionCode03;
		return ;
	}
	
	task = sendFunctionCode02;
}

void modbusParseFunctionCode02(const u8 *buf){
	u8 i;
	u8 data[48];
	for (i = 0; i < buf[2]; ++i){
		data[i] = buf[i + 3];
	}
	modbusHandFunctionCode02(data);
	
	task = sendFunctionCode03;
}

void sendFunctionCode02(u16 startAddr, u16 count){
	u16 temp;
	sendData.buf[0] = sendData.slavAddr;
	sendData.buf[1] = 0x02;
	sendData.buf[2] = startAddr >> 8;
	sendData.buf[3] = startAddr & 0xff;
	sendData.buf[4] = count >> 8;
	sendData.buf[5] = count & 0xff;
	temp = crc16(sendData.buf, 6);
	sendData.buf[6] = temp & 0xff;
	sendData.buf[7] = temp >> 8;
	sendData.count = 8;
	
	sendDataFunction();
}

void sendFunctionCode03(u16 startAddr, u16 count){
	u16 temp;
	sendData.buf[0] = sendData.slavAddr;
	sendData.buf[1] = 0x03;
	sendData.buf[2] = startAddr >> 8;
	sendData.buf[3] = startAddr & 0xff;
	sendData.buf[4] = count >> 8;
	sendData.buf[5] = count & 0xff;
	temp = crc16(sendData.buf, 6);
	sendData.buf[6] = temp & 0xff;
	sendData.buf[7] = temp >> 8;
	sendData.count = 8;
	
	sendDataFunction();
}

void sendFunctionCode16(u16 startAddr, u16 count){
	u16 temp;
	u16 data[16];
	u8 i;
	sendData.buf[0] = sendData.slavAddr;
	sendData.buf[1] = 0x10;
	sendData.buf[2] = startAddr >> 8;
	sendData.buf[3] = startAddr & 0xff;
	sendData.buf[4] = count >> 8;
	sendData.buf[5] = count & 0xff;
	sendData.buf[6] = count * 2;
	getFunctionCode16Data(data);
	for(i  = 0; i < count; ++i){
		sendData.buf[7 + 2*i] = data[i] >> 8;
		sendData.buf[8 + 2*i] = data[i] & 0xff;
	}
	temp = crc16(sendData.buf, 2 * count + 7);
	sendData.buf[7 + 2*count] = temp & 0xff;
	sendData.buf[8 + 2*count] = temp >> 8;
	sendData.count = 9 + 2 * count;
	
	sendDataFunction();	
}
