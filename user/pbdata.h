#ifndef _pbdata_H
#define _pbdata_H

#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_can.h"
#include "stm32f10x_dma.h"
#include <string.h>
#include "stdio.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "net.h"

#define true  (u8)1
#define false (u8)0
	
#define StartRecv		10
#define ProgressRecv	11
#define FinishRecv		12
#define WaitRecv		13
#define StartSend		14
#define ProgressSend	15
#define FinishSend		16

#define Port	5000

#define enableSend()
#define enableRecv()
	
typedef enum {ON = true, OFF = false} LedStatus;

typedef void (*FunctionCode)(u16, u16);

typedef struct {
	u8 buf[512];
	u16 count;
}_usartData;

typedef struct {
	_usartData data[8];
	u8 index;				//当前串口要发送的数据
	u8 count;
	u8 timerCount;
	u8 timerFlag;
}_usartListData;

extern _usartListData usart_recvData;
extern _usartListData usart_sendData;

typedef struct {
	u8 buf[1024];
	u16 count;
}_socketData;

extern _socketData socket_recvData[4];
extern _socketData socket_sendData[4];

#include "led.h"
#include "usart.h"
#include "tim.h"
#include "spi.h"
#include "iic.h"
#include "iicSimulator.h"
#include "at24c02.h"
#include "can.h"

//定义变量

extern u8 dt;

extern FunctionCode task;

//定义函数

void RCC_HSE_Configuration(void);
void delay(u32 nCount);
void delay_us(u32 nus);
void delay_ms(u16 nms);

extern void usartSaveData(const u8 byte);
extern void usartSendData(const u8 *buf, const u8 len);

extern void pushSocketSaveData(const uint8_t sn, const char *buf, const uint16_t len);
extern void popSocketRecvData(const uint8_t sn);
extern void popSocketSendData(const uint8_t sn);
extern void socketSendData(const uint8_t sn, const u8 *buf, const u8 len);

#endif
