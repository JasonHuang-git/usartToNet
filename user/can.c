#include "can.h"

static char txBuf[512] = {'\0'};
static int count = 0;
static int len = 0;

static CanTxMsg txMessage;

void canInit()
{
	canRCCInit();
	
	canGPIOConfigure();
	
	canConfigure();
	
	canNvicConfigure();
}

void canRCCInit()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
}

void canGPIOConfigure()
{
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = GPIO_Pin_8;
	gpioInit.GPIO_Mode = GPIO_Mode_IPU;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &gpioInit);
	
	gpioInit.GPIO_Pin = GPIO_Pin_9;
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &gpioInit);
}

void canConfigure()
{
	CAN_InitTypeDef can_init;
	CAN_DeInit(CAN1);
	CAN_StructInit(&can_init);
	
	//关闭时间出发模式
	can_init.CAN_TTCM = DISABLE;	
	//关闭自动离线管理
	can_init.CAN_ABOM = DISABLE;
	//关闭自动唤醒模式
	can_init.CAN_AWUM = DISABLE;
	//禁止报文自动重传
	can_init.CAN_NART = DISABLE;
	//FIFO 溢出时报文覆盖原文件
	can_init.CAN_RFLM = DISABLE;
	//报文发送优先级取决于ID号
	can_init.CAN_TXFP = DISABLE;
	//回环模式
	can_init.CAN_Mode = CAN_Mode_LoopBack;
	
	can_init.CAN_SJW = CAN_SJW_1tq;
	can_init.CAN_BS1 = CAN_BS1_3tq;
	can_init.CAN_BS2 = CAN_BS2_2tq;
	can_init.CAN_Prescaler = 48;
	
	CAN_Init(CAN1, &can_init);
	
	CAN_FilterInitTypeDef can_filter;
	
	//屏蔽滤波
	can_filter.CAN_FilterNumber = 0;
	//屏蔽模式
	can_filter.CAN_FilterMode = CAN_FilterMode_IdMask;
	//32位寄存器
	can_filter.CAN_FilterScale = CAN_FilterScale_32bit;
	//高16位
	can_filter.CAN_FilterIdHigh = 0x0000;
	//低16位
	can_filter.CAN_FilterIdLow = 0;
	//屏蔽位高16位
	can_filter.CAN_FilterMaskIdHigh = 0;
	//屏蔽位低16位
	can_filter.CAN_FilterMaskIdLow = 0;
	//过滤器0关联到FIFO0
	can_filter.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	//使能过滤器
	can_filter.CAN_FilterActivation = ENABLE;
	//初始化过滤器
	CAN_FilterInit(&can_filter);
	
	//使能接收中断
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

void canNvicConfigure()
{
	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 2;
	nvic_init.NVIC_IRQChannelSubPriority = 0;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
	
	nvic_init.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 2;
	nvic_init.NVIC_IRQChannelSubPriority = 0;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg rx;
	CAN_Receive(CAN1, CAN_FIFO0, &rx);
	for(int i = 0; i < rx.DLC; i++){
		usartPrintf("recv data: %c\r\n",rx.Data[i]);
	}
}

void USB_HP_CAN1_TX_IRQHandler(void)
{
	if(count < len){
		txMessage.Data[0] = txBuf[count++];
		CAN_Transmit(CAN1, &txMessage);
	}
	else {
		CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);
	}
}

void canWrite(const char *p)
{
	strcpy(txBuf, p);
	len = strlen(p);
	count = 0;
	txMessage.StdId = 0xFE00 >> 5;
	txMessage.ExtId = 0;
	txMessage.IDE = CAN_ID_STD;
	txMessage.RTR = CAN_RTR_DATA;
	txMessage.DLC =	1;	//此处最多为8
	txMessage.Data[0] = txBuf[count++];
	CAN_Transmit(CAN1, &txMessage);
	
	CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
}

