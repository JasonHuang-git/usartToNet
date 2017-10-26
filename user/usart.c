#include "usart.h"

static char sendBuf[256];
static volatile u8 sendFlag = true;

u8 recvBuf[256];

void usartInit(void)
{	
	usartRccInit();
	
	usartGPIOConfigure();
	
	dmaRccInit();
	
	dmaConfiguration();
	
	usartConfigure();
	
	nivcConfiguration();
	
	usartDataInit();
}

void usartDataInit(void){
	usart_recvData.count = 0;
	usart_recvData.index = 0;
	
	usart_sendData.count = 0;
	usart_sendData.index = 0;
}

void usartGPIOConfigure(void)
{
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GPIO_Pin_9;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio_init);
	
	gpio_init.GPIO_Pin = GPIO_Pin_10;
	gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio_init);
}

void usartConfigure(void)
{
	USART_InitTypeDef usart_init;
	usart_init.USART_BaudRate = 9600;
	usart_init.USART_WordLength = USART_WordLength_8b;
	usart_init.USART_StopBits = USART_StopBits_1;
	usart_init.USART_Parity = USART_Parity_No;
	usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART1, &usart_init);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

void usartRccInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

void nivcConfiguration(void)
{
	NVIC_InitTypeDef nvic_init;
	
	nvic_init.NVIC_IRQChannel = USART1_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_init.NVIC_IRQChannelSubPriority = 2;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic_init);
	
	nvic_init.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init.NVIC_IRQChannelSubPriority = 1;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic_init);
}

void dmaRccInit(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void dmaConfiguration(void){
	DMA_InitTypeDef dma_init;
	DMA_DeInit(DMA1_Channel4);
	
	dma_init.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
	dma_init.DMA_MemoryBaseAddr = (u32)sendBuf;
	dma_init.DMA_DIR = DMA_DIR_PeripheralDST;
	dma_init.DMA_BufferSize = 50;
	dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_init.DMA_M2M = DMA_M2M_Disable;
	dma_init.DMA_Priority = DMA_Priority_High;
	dma_init.DMA_Mode = DMA_Mode_Normal;
	
	DMA_Init(DMA1_Channel4, &dma_init);
	
	DMA_ClearFlag(DMA1_FLAG_GL4);
	
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
}

void USART1_IRQHandler(void)
{	
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{
		usartSaveData(USART_ReceiveData(USART1));		
	}
	
//	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET){
//		USART_ClearFlag(USART1, USART_FLAG_TC);
//	}
}

void sendData(void){
	if(sendFlag == true && usart_sendData.count != 0){
		sendFlag = false;
		strncpy(sendBuf, (char *)usart_sendData.data[usart_sendData.index].buf, usart_sendData.data[usart_sendData.index].count);
		DMA_Cmd(DMA1_Channel4, DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel4, usart_sendData.data[usart_sendData.index].count);
		DMA_Cmd(DMA1_Channel4, ENABLE);
		
		usart_sendData.data[usart_sendData.index].count = 0;
		usart_sendData.count--;
		usart_sendData.index++;
		if(usart_sendData.index > 7){
			usart_sendData.index = 0;
		}
	}
}

void usartPrintf(char *fmt,...)
{
	va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    putStr(string);
	putStr("\r\n");
    va_end(ap);
}

void putStr(char *str){
// 未使用dma发送时
//	while(*str != '\0'){
//		USART_SendData(USART1, *str++);
//		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
//	}
	usartSendData(str, strlen(str));
	sendData();
//	if(sendFlag == true){
//		sendFlag = false;
//		strcpy(sendBuf, str);
//		DMA_Cmd(DMA1_Channel4, DISABLE);
//		DMA_SetCurrDataCounter(DMA1_Channel4, strlen(str));
//		DMA_Cmd(DMA1_Channel4, ENABLE);
//	}
}

void putChar(char ch){
	while(!sendFlag);
	sendFlag = false;
	strncpy(sendBuf, &ch, 1);
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel4, 1);
	DMA_Cmd(DMA1_Channel4, ENABLE);
}

void DMA1_Channel4_IRQHandler(void){
	sendFlag = true;
	DMA_ClearFlag(DMA1_IT_TC4);
	DMA_Cmd(DMA1_Channel4, DISABLE);
	
	sendData();
}
