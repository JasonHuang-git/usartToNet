#include "iic.h"

char txBuf[1024] = {'\0'};
int writeCount = 0;
int writeLen = 0;
char recvBuf[1024] = {'\0'};
int readCount = 0;
int readAddr = 0;

void iicInit(void)
{
	iicRccInit();
	
	iicGPIOInit();
	
	iicConfigure();
	
	iicNvicConfigure();
}

void iicRccInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
}

void iicGPIOInit(void)
{
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GPIO_Pin_6;	//SCL
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOB, &gpio_init);
	
	gpio_init.GPIO_Pin = GPIO_Pin_7;	//SDA
	GPIO_Init(GPIOB, &gpio_init);
}

void iicConfigure(void)
{
	I2C_InitTypeDef iic_init;
	iic_init.I2C_Ack = I2C_Ack_Enable;
	iic_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	iic_init.I2C_ClockSpeed = ClockSpeedKHz(300);
	iic_init.I2C_DutyCycle = I2C_DutyCycle_2;
	iic_init.I2C_Mode = I2C_Mode_I2C;
	iic_init.I2C_OwnAddress1 = 11;
	
	I2C_Init(I2C1, &iic_init);
	
//	I2C_ITConfig(I2C2, I2C_IT_BUF, ENABLE);
//	I2C_ITConfig(I2C2, I2C_IT_EVT, ENABLE);
//	I2C_ITConfig(I2C2, I2C_IT_ERR, ENABLE);
	
	I2C_Cmd(I2C1, ENABLE);
}

void iicNvicConfigure(void)
{
	NVIC_InitTypeDef nvic_init;
		
	nvic_init.NVIC_IRQChannel = I2C1_EV_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init.NVIC_IRQChannelSubPriority = 0;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic_init);
	
	nvic_init.NVIC_IRQChannel = I2C1_ER_IRQn;
	nvic_init.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic_init);
}

void I2C2_EV_IRQHandler(void)
{
	//发送起始位成功
	uint32_t event = I2C_GetLastEvent(I2C2);
	printf("i2c ev irqhandler!\r\n");
	switch(event){
	case I2C_EVENT_MASTER_MODE_SELECT:
		I2C_AcknowledgeConfig(I2C2, DISABLE);
		I2C_Send7bitAddress(I2C2, SlaveDevice, I2C_Direction_Transmitter);
		printf("send start bit success!\r\n");
	break;
	case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
		ledOn();
		I2C_SendData(I2C2, txBuf[writeCount++]);
		if(writeLen == 1){
			I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
		}
		printf("send the device for write success!\r\n");
	break;
	case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
		printf("send the device for read success!\r\n");
	break;
	case I2C_EVENT_MASTER_MODE_ADDRESS10:
		printf("send the device 10 success!\r\n");
	break;
	case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
		if(writeCount < writeLen){
			I2C_SendData(I2C2, txBuf[writeCount++]);
		}
		else {
			I2C_ITConfig(I2C2, I2C_IT_BUF, DISABLE);
		}
		printf("send the byte transmiting!\r\n");
	break;
	case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
		printf("send the byte transmited!\r\n");
	break;
	case I2C_EVENT_MASTER_BYTE_RECEIVED:
		printf("read the byte transmited!\r\n");
	break;
	}
}

void I2C2_ER_IRQHandler(void)
{
	uint32_t event = I2C_GetLastEvent(I2C2);
	printf("i2c ev irqhandler, %X!\r\n", event);
//	switch(event){
//		case I2C_
//	}
	if(I2C_GetFlagStatus(I2C2, I2C_IT_BERR) != RESET){
		printf("the bus error!\r\n");
	}
	else if(I2C_GetFlagStatus(I2C2, I2C_IT_ARLO) != RESET){
		printf("Arbitration is missing!\r\n");
	}
	else if(I2C_GetFlagStatus(I2C2,I2C_IT_AF) != RESET){
		printf("In response to faliure!\r\n");
	}
	else if(I2C_GetFlagStatus(I2C2, I2C_IT_OVR) != RESET){
		printf("ovr error\r\n");
	}
	else if(I2C_GetFlagStatus(I2C2, I2C_IT_PECERR) != RESET){
		printf("PEC error!\r\n");
	}
	else if(I2C_GetFlagStatus(I2C2, I2C_IT_TIMEOUT) != RESET){
		printf("timout error!\r\n");
	}
	else if(I2C_GetFlagStatus(I2C2,  I2C_IT_SMBALERT) != RESET){
		printf("smb alert error!\r\n");
	}
}

void writeData(char *data, int len, int addr)
{
	txBuf[0] = addr;
	strncpy(&txBuf[1], data, len);
	writeLen = len;
	writeCount = 0;

	printf("write data end\r\n");
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {} 

	/* Disable Acknowledgement */
//  I2C_AcknowledgeConfig(I2C1, DISABLE);
	  printf("write event master mode select\r\n");
	 
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2C1, 0xA0, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {}
	printf("write event master transmitter mode selected.\r\n");
}
