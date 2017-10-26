#include "tim.h"

void tim3Init(void)
{
	tim3RccInit();
	
	tim3Configure();
	
	tim3NvicConfigure();
}

void tim3Configure(void)
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	
	TIM_TimeBaseInitTypeDef timeInit;
	
	timeInit.TIM_Period = 2 * TIM3_TIMING_MS;
	timeInit.TIM_Prescaler = 36000 - 1;
	timeInit.TIM_ClockDivision = TIM_CKD_DIV1;
	timeInit.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &timeInit);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
}

void tim3RccInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

void tim3NvicConfigure(void)
{
	NVIC_InitTypeDef nvic_init;
	
	nvic_init.NVIC_IRQChannel = TIM3_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_init.NVIC_IRQChannelSubPriority = 3;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic_init);
}

void TIM3_IRQHandler(void)
{
	static int count = 0;
	if(count++ < 500){
		ledOff();
	}
	else if(count < 1000){
		ledOn();
	}
	else {
		count = 0;
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	if(usart_recvData.timerFlag == StartRecv){
		usart_recvData.timerCount++;
		if(usart_recvData.timerCount > 20){
			usart_recvData.timerFlag = FinishRecv;
		}
	}
}
