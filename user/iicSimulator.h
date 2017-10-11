#ifndef _IICSIMULATOR_H
#define _IICSIMULATOR_H

#include "pbdata.h"

#define I2C_SCL GPIO_Pin_10
#define I2C_SDA GPIO_Pin_11
#define GPIO_I2C GPIOB

#define I2C_SCL_H() GPIO_SetBits(GPIO_I2C, I2C_SCL)
#define I2C_SCL_L() GPIO_ResetBits(GPIO_I2C, I2C_SCL)

#define I2C_SDA_H() GPIO_SetBits(GPIO_I2C, I2C_SDA)
#define I2C_SDA_L() GPIO_ResetBits(GPIO_I2C, I2C_SDA)

extern void iicSimulatorInit(void);
extern void iicStart(void);
extern void iicStop(void);
extern void iicACK(void);
extern void  iicNACK(void);
extern u8 iicWaitAck(void);
extern void iicSendByte(u8 dat);
extern u8 iicReadByte(u8 ack);

#endif
