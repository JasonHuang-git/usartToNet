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
	
	//�ر�ʱ�����ģʽ
	can_init.CAN_TTCM = DISABLE;	
	//�ر��Զ����߹���
	can_init.CAN_ABOM = DISABLE;
	//�ر��Զ�����ģʽ
	can_init.CAN_AWUM = DISABLE;
	//��ֹ�����Զ��ش�
	can_init.CAN_NART = DISABLE;
	//FIFO ���ʱ���ĸ���ԭ�ļ�
	can_init.CAN_RFLM = DISABLE;
	//���ķ������ȼ�ȡ����ID��
	can_init.CAN_TXFP = DISABLE;
	//�ػ�ģʽ
	can_init.CAN_Mode = CAN_Mode_LoopBack;
	
	can_init.CAN_SJW = CAN_SJW_1tq;
	can_init.CAN_BS1 = CAN_BS1_3tq;
	can_init.CAN_BS2 = CAN_BS2_2tq;
	can_init.CAN_Prescaler = 48;
	
	CAN_Init(CAN1, &can_init);
	
	CAN_FilterInitTypeDef can_filter;
	
	//�����˲�
	can_filter.CAN_FilterNumber = 0;
	//����ģʽ
	can_filter.CAN_FilterMode = CAN_FilterMode_IdMask;
	//32λ�Ĵ���
	can_filter.CAN_FilterScale = CAN_FilterScale_32bit;
	//��16λ
	can_filter.CAN_FilterIdHigh = 0x0000;
	//��16λ
	can_filter.CAN_FilterIdLow = 0;
	//����λ��16λ
	can_filter.CAN_FilterMaskIdHigh = 0;
	//����λ��16λ
	can_filter.CAN_FilterMaskIdLow = 0;
	//������0������FIFO0
	can_filter.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	//ʹ�ܹ�����
	can_filter.CAN_FilterActivation = ENABLE;
	//��ʼ��������
	CAN_FilterInit(&can_filter);
	
	//ʹ�ܽ����ж�
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
	txMessage.DLC =	1;	//�˴����Ϊ8
	txMessage.Data[0] = txBuf[count++];
	CAN_Transmit(CAN1, &txMessage);
	
	CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
}

