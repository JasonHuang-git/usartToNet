#include  "at24c02.h"

u8 AT24Cxx_ReadOneByte(u16 addr)
{
	u8 temp=0;

	iicStart();
	
	iicSendByte(0xA0);
	iicWaitAck();
	iicSendByte(addr>>8);

	iicWaitAck();
	iicSendByte(addr%256);	
							
	iicWaitAck();

	iicStart();
	iicSendByte(0xA1);
	iicWaitAck();

	temp=iicReadByte(0); //  0   代表 NACK
	iicStop();
	
	return temp;	
}

u16 AT24Cxx_ReadTwoByte(u16 addr)
{
	u16 temp=0;

	iicStart();

	iicSendByte(0xA0);
	iicWaitAck();
	iicSendByte(addr>>8);	//发送数据地址高位

	iicWaitAck();
	iicSendByte(addr%256);//双字节是数据地址低位		
							//单字节是数据地址低位
	iicWaitAck();

	iicStart();
	iicSendByte(0xA1);
	iicWaitAck();

	temp=iicReadByte(1); //  1   代表 ACK
	temp<<=8;
	temp|=iicReadByte(0); //  0  代表 NACK

	iicStop();
	
	return temp;	
}


void AT24Cxx_WriteOneByte(u16 addr,u8 dt)
{
	iicStart();

	iicSendByte(0xA0);
	iicWaitAck();
	iicSendByte(addr>>8);	//发送数据地址高位

	iicWaitAck();
	iicSendByte(addr%256);//双字节是数据地址低位		
							//单字节是数据地址低位
	iicWaitAck();

	iicSendByte(dt);
	iicWaitAck();
	iicStop();
}

void AT24Cxx_WriteTwoByte(u16 addr,u16 dt)
{
	iicStart();

	iicSendByte(0xA0);
	iicWaitAck();
	iicSendByte(addr>>8);	//发送数据地址高位

	iicWaitAck();
	iicSendByte(addr%256);//双字节是数据地址低位		
							//单字节是数据地址低位
	iicWaitAck();

	iicSendByte(dt>>8);
	iicWaitAck();

	iicSendByte(dt&0xFF);
	iicWaitAck();

	iicStop();

	delay_ms(10);
}

