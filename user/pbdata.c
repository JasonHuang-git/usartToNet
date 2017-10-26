#include "pbdata.h"

u8 dt = 0;

_usartListData usart_recvData;
_usartListData usart_sendData;

_socketData socket_recvData[4];
_socketData socket_sendData[4];

FunctionCode task;

void RCC_HSE_Configuration(void) //HSE作为PLL时钟，PLL作为SYSCLK 
{ 
   RCC_DeInit(); /*将外设RCC寄存器重设为缺省值 */ 
   RCC_HSEConfig(RCC_HSE_ON); 	 /*设置外部高速晶振（HSE） HSE晶振打开(ON)*/ 

   if(RCC_WaitForHSEStartUp() == SUCCESS) {  /*等待HSE起振,  SUCCESS：HSE晶振稳定且就绪*/  

   RCC_HCLKConfig(RCC_SYSCLK_Div1);/*设置AHB时钟(HCLK)RCC_SYSCLK_Div1——AHB时钟 = 系统时*/  
   RCC_PCLK2Config(RCC_HCLK_Div1); /*设置高速AHB时钟(PCLK2)RCC_HCLK_Div1——APB2时钟 = HCLK*/     
   RCC_PCLK1Config(RCC_HCLK_Div2); /*设置低速AHB时钟(PCLK1)RCC_HCLK_Div2——APB1时钟 = HCLK / 2*/      

   RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);/*设置PLL时钟源及倍频系数*/    
   RCC_PLLCmd(ENABLE); 	 /*使能PLL */ 
   while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) ; /*检查指定的RCC标志位(PLL准备好标志)设置与否*/ 
    
   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  /*设置系统时钟（SYSCLK） */  
   while(RCC_GetSYSCLKSource() != 0x08);     /*0x08：PLL作为系统时钟 */	    
 } 
} 

void delay(u32 nCount)
{
	for(;nCount!=0;nCount--);
}

/****************************************************************************
* 名    称：delay_us(u32 nus)
* 功    能：微秒延时函数
* 入口参数：u32  nus
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void delay_us(u32 nus)
{
	 u32 temp;
	 SysTick->LOAD = 9*nus;
	 SysTick->VAL=0X00;//清空计数器
	 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
	 do
	 {
	  temp=SysTick->CTRL;//读取当前倒计数值
	 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
	 
	 SysTick->CTRL=0x00; //关闭计数器
	 SysTick->VAL =0X00; //清空计数器
}

/****************************************************************************
* 名    称：delay_ms(u16 nms)
* 功    能：毫秒延时函数
* 入口参数：u16 nms
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void delay_ms(u16 nms)
{
	 u32 temp;
	 SysTick->LOAD = 9000*nms;
	 SysTick->VAL=0X00;//清空计数器
	 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
	 do
	 {
	  temp=SysTick->CTRL;//读取当前倒计数值
	 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
	 SysTick->CTRL=0x00; //关闭计数器
	 SysTick->VAL =0X00; //清空计数器
}

//功能码03读到的数据处理；
void modbusHandFunctionCode03(const u16 *buf){
	
}

//功能码02读到的数据处理；
void modbusHandFunctionCode02(const u8 *buf){
	
}

//功能码16要发送的数据，填充到buf里面；
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
	usartPrintf("index %d, count:%d, %d", usart_recvData.index, usart_recvData.count, count);
	usart_recvData.timerFlag = StartRecv;
	usart_recvData.timerCount = 0;
	if(count < 10){
		usart_recvData.data[usart_recvData.index + usart_recvData.count].buf[count] = byte;
		usart_recvData.data[usart_recvData.index + usart_recvData.count].count++;
	}
	else {
		usart_recvData.timerFlag = FinishRecv;
	}
}

void usartSendData(const u8 *buf, const u8 len){
	int index = usart_sendData.index + usart_sendData.count;
	if(index > 7){
		index = 7;
	}
	printf("index: %d,%d\r\n", index,len);
	strncpy((char *)usart_sendData.data[index].buf, (char *)buf, len);
	usart_sendData.data[index].count = len;
	usart_sendData.count++;	
	if(usart_sendData.count >= 8){
		usart_sendData.count = 8;
	}	
}

void pushSocketSaveData(const uint8_t sn, const char *buf, const uint16_t len){
	int length = socket_recvData[sn].count + len > 1024 ? 1024 - socket_recvData[sn].count : len;
	strncpy((char *)&socket_recvData[sn].buf[socket_recvData[sn].count], (char *)buf, length);
	socket_recvData[sn].count += length;
	usartPrintf("push socket save data: %d", length);
} 

void popSocketRecvData(const uint8_t sn){
	socket_recvData[sn].count = 0;
}

void popSocketSendData(const uint8_t sn){
	socket_sendData[sn].count = 0;
}

void socketSendData(const uint8_t sn, const u8 *buf, const u8 len){
	int length = socket_sendData[sn].count + len > 1024 ? 1024 - socket_sendData[sn].count : len;
	strncpy((char *)&socket_sendData[sn].buf[socket_sendData[sn].count], (char *)buf, length);
	socket_sendData[sn].count += length;
}
