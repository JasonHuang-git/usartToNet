#include "led.h"

void ledInit(void)
{
	ledRccInit();
	
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GPIO_Pin_0;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOA, &gpio_init);
}

void ledRccInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

void ledOn(void)
{
	if(ledStatus() == OFF){
		GPIO_SetBits(GPIOA,GPIO_Pin_0);
	}
}

void ledOff(void)
{
	if(ledStatus() == ON){
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	}
}

LedStatus ledStatus(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET){
		return OFF;
	}
	return ON;
}
