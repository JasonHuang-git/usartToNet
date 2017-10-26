#include "spi.h"

static u8 spiRecv[1024] = {'\0'};
static u8 spiSend[1024] = {'\0'};
volatile u16 recvCount = 0;
volatile u16 sendCount = 0;
int sendSize = 0;

void spiInit(void){	
	spiRccinit();
	
	spiGPIOInit();
	
	spiConfigure();
	
	spiNivcConfiguration();
	
	initData();
}

void spiGPIOInit(void){
	//SPI1����Ϊ��ģʽ��
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GPIO_Pin_1;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &gpio_init);
	
	gpio_init.GPIO_Pin = GPIO_Pin_5;
	gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio_init);
	
	gpio_init.GPIO_Pin = GPIO_Pin_7;  //MOSI ���
	GPIO_Init(GPIOA, &gpio_init);
	
	gpio_init.GPIO_Pin = GPIO_Pin_6;		//MISO����
	GPIO_Init(GPIOA, &gpio_init);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void spiConfigure(void){
	SPI_InitTypeDef spi_Init;
	spi_Init.SPI_CPHA = SPI_CPHA_1Edge;
	spi_Init.SPI_CPOL = SPI_CPOL_Low;
	spi_Init.SPI_DataSize = SPI_DataSize_8b;
	spi_Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	spi_Init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi_Init.SPI_FirstBit = SPI_FirstBit_MSB;
	spi_Init.SPI_Mode = SPI_Mode_Master;
	spi_Init.SPI_NSS = SPI_NSS_Soft;
	spi_Init.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI1, &spi_Init);
	
//	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE); 
    SPI_Cmd(SPI1,ENABLE);
}


void spiRccinit(void){	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
}

void spiNivcConfiguration(void){
	NVIC_InitTypeDef nvic_init;
	
	nvic_init.NVIC_IRQChannel = SPI1_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 2;
	nvic_init.NVIC_IRQChannelSubPriority = 1;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic_init);
}


void initData(void){	
}

void SPI1_IRQHandler(void){
	if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != RESET){
		if(spiSend[++sendCount] != '\0'){
			SPI_I2S_SendData(SPI1, spiSend[sendCount]);
		}
		else {		
			SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE, DISABLE);
		}
	}
	if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET){
		spiRecv[recvCount] = SPI_I2S_ReceiveData(SPI1);
		if(recvCount == 1023){
			recvCount = 0;
		}
		if(spiRecv[recvCount] == '*'){
			spiRecv[recvCount + 1] = '\0';
			printf("recv data: %s\r\n", spiRecv);
			recvCount = 0;
		}
		else {
			recvCount++;
		}
	}
}

void spiWrite(const char *p){
	strcpy((char *)spiSend, p);
	sendCount = 0;
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, spiSend[sendCount]);
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE, ENABLE);
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//	recv[0] = SPI_I2S_ReceiveData(SPI1);
//	recv[1] = '\0';
//	printf("recv ok! ");
//	printf(recv);
}

void SPI_WriteByte(uint8_t data){
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(SPI1);
}

uint8_t SPI_ReadByte(void){
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, 0xFF);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);
}

void SPI_CrisEnter(void){
	__set_PRIMASK(1);
}

void SPI_CrisExit(void){
	__set_PRIMASK(0);
}

void SPI_CS_Select(void){
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void SPI_CS_Deselect(void){
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void reg_SPI_W5500(void){
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);
	
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);
	
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);
}

