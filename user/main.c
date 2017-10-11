#include "pbdata.h"

int fputc(int ch,FILE *f)
{
//	USART_SendData(USART1,(u8)ch);
//	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	putChar(ch);
	return ch;
}

void keyInit()
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_5;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_Init(GPIOC, &gpio_init);
}

void ExtiConfiguration(void)
{
	EXTI_InitTypeDef exti_init;
	
	EXTI_ClearITPendingBit(EXTI_Line5);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
	
	exti_init.EXTI_Line = EXTI_Line5;
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_init.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&exti_init);
}

int main(void)
{	
	u16 dat = 0;
	
	SystemInit();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
	ledInit();
	
	keyInit();
	
	tim3Init();
	
	usartInit();
	
	spiInit();
	
//	iicInit();
	iicSimulatorInit();
	
	canInit();
	
	delay_ms(5000);
	
	ledOff();
	
	AT24Cxx_WriteTwoByte(20, 0x3245);
	
	while(true){
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) == Bit_RESET){
			canWrite("can");
//			printf("m");
//			printf("d");
//			printf("1");
//			delay_ms(1000);
			ledOn();
		}
		else {
			ledOff();
		}
	}
	
//	writeData("hello iic", strlen("hello iic"), 0);
	
//	while(true){
//		dat = AT24Cxx_ReadTwoByte(20);
//		printf("read addr:%d data is %X.\r\n", 20,  dat);
//		delay_ms(5000);
//		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) == Bit_RESET){
////			spiWrite("123*");
//			dat = AT24Cxx_ReadTwoByte(20);
//			printf("read addr:%d data is %X.\r\n", 20,  dat);
//			delay_ms(5000);
//			
//		}
//		else {
//			ledOff();
//		}
//	}
}
