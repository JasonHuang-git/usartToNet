#include "iicSimulator.h"

void iicSimulatorInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = I2C_SCL | I2C_SDA;
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_I2C, &gpioInit);
	
	I2C_SCL_H();
	I2C_SDA_H();
}

void iicSDAOut(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_I2C,&GPIO_InitStructure);
}

void iicSDAIN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void iicStart(void)
{
	iicSDAOut();
	
	I2C_SDA_H();
	I2C_SCL_H();
	delay_us(5);
	I2C_SDA_L();
	delay_us(5);
	I2C_SCL_L();
}

void iicStop(void)
{
	iicSDAOut();
	
	I2C_SCL_L();
	I2C_SDA_L();
	I2C_SCL_H();
	
	delay_us(5);
	I2C_SDA_H();
	delay_us(6);
}

void iicACK(void)
{
	I2C_SCL_L();
	iicSDAOut();
	I2C_SDA_L();
	delay_us(2);
	I2C_SCL_H();
	delay_us(5);
	I2C_SCL_L();
}

void  iicNACK(void)
{
	I2C_SCL_L();
	iicSDAOut();
	I2C_SDA_H();
	delay_us(2);
	I2C_SCL_H();
	delay_us(5);
	I2C_SCL_L();	
}

u8 iicWaitAck(void)
{
	u8 timeout = 0;
	iicSDAIN();
	
	I2C_SDA_H();
	delay_us(1);
	I2C_SCL_H();
	delay_us(1);
	
	while(GPIO_ReadInputDataBit(GPIO_I2C,  I2C_SDA)){
		if(timeout++ > 250){
			iicStop();
			return 1;
		}
	}
	I2C_SCL_L();
	return 0;
}

void iicSendByte(u8 dat)
{
	u8 i = 0;
	
	iicSDAOut();
	I2C_SCL_L();
	for(i = 0; i < 8; i++){
		if(dat & 0x80){
			I2C_SDA_H();
		}
		else {
			I2C_SDA_L();
		}
		dat <<= 1;
		I2C_SCL_H();
		delay_us(2);
		I2C_SCL_L();
		delay_us(2);
	}
}

u8 iicReadByte(u8 ack)
{
	u8 i = 0, dat = 0;
	iicSDAIN();
	for(i = 0; i < 8; i++){
		I2C_SCL_L();
		delay_us(2);
		I2C_SCL_H();
		dat <<= 1;
		if(GPIO_ReadInputDataBit(GPIO_I2C, I2C_SDA)){
			dat++;
		}
		delay_us(1);
	}
	if(ack){
		iicACK();
	}
	else {
		iicNACK();
	}
	return dat;
}
