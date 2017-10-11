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

#define true  (u8)1
#define false (u8)0
	
#define StartRecv		10
#define ProgressRecv	11
#define FinishRecv		12
#define StartSend		13
#define ProgressSend	14
#define FinishSend		15

#define enableSend()
#define enableRecv()
	
typedef enum {ON = true, OFF = false} LedStatus;

typedef void (*FunctionCode)(u16, u16);

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

#endif
