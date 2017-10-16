#include "pbdata.h"
#include "loopback.h"

int fputc(int ch,FILE *f)
{
//	USART_SendData(USART1,(u8)ch);
//	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	putChar(ch);
	return ch;
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
	
	uint8_t buf[1024] = {'\0'};
	
	SystemInit();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
	ledInit();
	
	tim3Init();
	
	usartInit();
	
	spiInit();
	
	delay_ms(5000);
	
	ledOn();
	
	reg_SPI_W5500();
	
	network_Init();
	
	setKeepAlive(0);
	
	setRTR(2000);
	
	setRCR(3);
	
	
	
	while(true){	
		printf("ret = %d\r\n", loopback_tcps(0, buf, 5000));
		delay_ms(1000);
	}
}
