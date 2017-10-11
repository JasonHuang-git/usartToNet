#include "led.h"

void ledInit(void)
{
	ledRccInit();
	
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOD, &gpio_init);
}

void ledRccInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
}

void ledOn(void)
{
	if(ledStatus() == OFF){
		GPIO_ResetBits(GPIOD,GPIO_Pin_3);
	}
}

void ledOff(void)
{
	if(ledStatus() == ON){
		GPIO_SetBits(GPIOD, GPIO_Pin_3);
	}
}

LedStatus ledStatus(void)
{
	if(GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_3) == Bit_RESET){
		return ON;
	}
	return OFF;
}
