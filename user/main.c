#include "pbdata.h"

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

void network_Init(void)
{
	uint8_t tmpstr[6];
	uint8_t ip[] = {192,168,1,254};
	uint8_t gw[] = {192,168,1,1};
	uint8_t sn[] = {255,255,255,255};
	uint8_t mac[] = {0x86, 0x6b, 0x58, 0x46, 0x6a, 0xf5};
	wiz_NetInfo gWIZNETINFO;
	for(int i = 0; i < 4; i++){
		gWIZNETINFO.ip[i] = ip[i];
		gWIZNETINFO.gw[i] = gw[i];
		gWIZNETINFO.sn[i] = sn[i];
	}
	for(int i = 0; i < 6; i++){
		gWIZNETINFO.mac[i] = mac[i];
	}
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
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
	
	while(true){
		loopback_tcps(0, buf, 5000);
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
