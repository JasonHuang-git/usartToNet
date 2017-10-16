#include "pbdata.h"

u8 dt = 0;

_usartListData usart_recvData;
_usartListData usart_sendData;

_socketListData socket_recvData[8];
_socketListData socket_sendData[8];

FunctionCode task;

void RCC_HSE_Configuration(void) //HSE��ΪPLLʱ�ӣ�PLL��ΪSYSCLK 
{ 
   RCC_DeInit(); /*������RCC�Ĵ�������Ϊȱʡֵ */ 
   RCC_HSEConfig(RCC_HSE_ON); 	 /*�����ⲿ���پ���HSE�� HSE�����(ON)*/ 

   if(RCC_WaitForHSEStartUp() == SUCCESS) {  /*�ȴ�HSE����,  SUCCESS��HSE�����ȶ��Ҿ���*/  

   RCC_HCLKConfig(RCC_SYSCLK_Div1);/*����AHBʱ��(HCLK)RCC_SYSCLK_Div1����AHBʱ�� = ϵͳʱ*/  
   RCC_PCLK2Config(RCC_HCLK_Div1); /*���ø���AHBʱ��(PCLK2)RCC_HCLK_Div1����APB2ʱ�� = HCLK*/     
   RCC_PCLK1Config(RCC_HCLK_Div2); /*���õ���AHBʱ��(PCLK1)RCC_HCLK_Div2����APB1ʱ�� = HCLK / 2*/      

   RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);/*����PLLʱ��Դ����Ƶϵ��*/    
   RCC_PLLCmd(ENABLE); 	 /*ʹ��PLL */ 
   while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) ; /*���ָ����RCC��־λ(PLL׼���ñ�־)�������*/ 
    
   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  /*����ϵͳʱ�ӣ�SYSCLK�� */  
   while(RCC_GetSYSCLKSource() != 0x08);     /*0x08��PLL��Ϊϵͳʱ�� */	    
 } 
} 

void delay(u32 nCount)
{
	for(;nCount!=0;nCount--);
}

/****************************************************************************
* ��    �ƣ�delay_us(u32 nus)
* ��    �ܣ�΢����ʱ����
* ��ڲ�����u32  nus
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void delay_us(u32 nus)
{
	 u32 temp;
	 SysTick->LOAD = 9*nus;
	 SysTick->VAL=0X00;//��ռ�����
	 SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
	 do
	 {
	  temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
	 }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
	 
	 SysTick->CTRL=0x00; //�رռ�����
	 SysTick->VAL =0X00; //��ռ�����
}

/****************************************************************************
* ��    �ƣ�delay_ms(u16 nms)
* ��    �ܣ�������ʱ����
* ��ڲ�����u16 nms
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void delay_ms(u16 nms)
{
	 u32 temp;
	 SysTick->LOAD = 9000*nms;
	 SysTick->VAL=0X00;//��ռ�����
	 SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
	 do
	 {
	  temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
	 }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
	 SysTick->CTRL=0x00; //�رռ�����
	 SysTick->VAL =0X00; //��ռ�����
}

//������03���������ݴ���
void modbusHandFunctionCode03(const u16 *buf){
	
}

//������02���������ݴ���
void modbusHandFunctionCode02(const u8 *buf){
	
}

//������16Ҫ���͵����ݣ���䵽buf���棻
void getFunctionCode16Data(u16 *buf){
	
}

void executeTask(const u8 cmd)
{
	switch(cmd){
	case 0x02: task(0, 12);break;
	case 0x03: task(2, 12);break;
	case 0x10: task(1, 10);break;
	}
}

void usartSaveData(const u8 byte)
{
	u16 count = usart_recvData.data[usart_recvData.index + usart_recvData.count].count;
	usart_recvData.timerFlag = StartRecv;
	usart_recvData.timerCount = 0;
	if(count < 512){
		usart_recvData.data[usart_recvData.index + usart_recvData.count].buf[count] = byte;
		usart_recvData.data[usart_recvData.index + usart_recvData.count].count++;
	}
	else {
		usart_recvData.timerFlag = FinishRecv;
	}
}

void usartSendData(const u8 *buf, const u8 len){
	int index = usart_sendData.index + usart_sendData.count;
	if(usart_sendData.index + usart_sendData.count > 7){
		index = 7 - index;
	}
	strncpy((char *)usart_sendData.data[index].buf, (char *)buf, len);
	usart_sendData.count++;	
	if(usart_sendData.count >= 8){
		usart_sendData.count = 0;
	}
	
	
}
