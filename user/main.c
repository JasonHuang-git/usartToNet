#include "pbdata.h"
#include "loopback.h"

int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
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
	
	uint8_t buf[2048] = {'\0'};
	uint16_t len = 0;
	uint8_t index = 0;
	
	SystemInit();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
	ledInit();
	
	tim3Init();
	
	usartInit();
	
	spiInit();
	
	delay_ms(5000);
	
	ledOn();
	
	reg_SPI_W5500();
	
	net_Init();
	
	while(true){
		usartPrintf("hello usart\r\n");
		delay_ms(1000);
		//串口接收数据完毕，切还到下一个缓存区
//		if(FinishRecv == usart_recvData.timerFlag){
//			usart_recvData.count++;
//			usart_recvData.timerFlag = WaitRecv;
//			usartPrintf("hello");
//		}
		//串口接收缓存区中有数据，转存到socket发送缓存区中，等待发送
//		if(usart_recvData.count != 0){
//			for(int i = 0; i < 4; i++){
//				net_SendData(i, usart_recvData.data[usart_recvData.index].buf, usart_recvData.data[usart_recvData.index].count);
////				socketSendData(i, usart_recvData.data[usart_recvData.index].buf, usart_recvData.data[usart_recvData.index].count);				
//			}
//			usart_recvData.data[usart_recvData.index].count = 0;
//			usart_recvData.index++;
//			usart_recvData.count--;
//			if(usart_recvData.index > 7){
//				usart_recvData.index = 0;				
//			}
//		}
//		//socket接收到数据，存储到socket接收缓存区中，等待处理
//		for(int i = 0; i < 4; i++){
//			if((len = net_recvData(i, buf)) != (uint16_t )-1){
//				usartPrintf("recv data: %d\r\n", len);
//				usartSendData(buf, len);
//			}		
//		}
//		sendData();
	}
}
