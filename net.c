#include "pbdata.h"

void net_Init(void){
	
}

void setNetWorkDefault(void)
{
	uint8_t tmpstr[6];
	uint8_t ip[] = {192,168,1,254};
	uint8_t gw[] = {192,168,1,1};
	uint8_t sn[] = {255,255,255,0};
	uint8_t mac[] = {0x86, 0x6b, 0x58, 0x46, 0x6a, 0xf5};
	uint8_t dns[] = {0, 0, 0, 0};
	wiz_NetInfo gWIZNETINFO;
	for(int i = 0; i < 4; i++){
		gWIZNETINFO.ip[i] = ip[i];
		gWIZNETINFO.gw[i] = gw[i];
		gWIZNETINFO.sn[i] = sn[i];
		gWIZNETINFO.dns[i] = dns[i];
	}
	for(int i = 0; i < 6; i++){
		gWIZNETINFO.mac[i] = mac[i];
	}
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	
	ctlwizchip(CW_GET_ID, (void *)tmpstr);
	
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
          gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
    printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
    printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
    printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
}

void phy_init(){
	uint8_t tmp;
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2}, {2,2,2,2,2,2,2,2}};
	
	if(ctlwizchip(CW_INIT_WIZCHIP, (void*)memsize) == -1){
		printf("wizchip initialized fail.\r\n");
		return ;
	}	
	do {
		if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){
			printf("cw get phylink failed.\r\n");
		}
	}while(tmp == PHY_LINK_OFF);
}

void setKeepAlive(SOCKET s){
	IINCHIP_WRITE(Sn_KPALVTR(s), 0x02);
}

uint32_t server_Init(uint8_t sn, int port){
	int32_t ret = 0;
	if(getSn_SR(sn) == SOCK_CLOSED){
		if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn){
			printf("%d:Socket opened failed\r\n", sn);
			return ret;
		}
	}
	if(getSn_SR(sn) == SOCK_INIT){
		if((ret = listen(sn)) != SOCK_OK){
			printf("%d:Socket Listen failed\r\n", sn);
			return ret;
		}
	}
	return 1;
}

uint32_t net_SendData(uint8_t sn, uint8_t* buf, uint16_t len){
	int32_t ret;
	uint16_t size = 0, sentsize=0;

#ifdef _LOOPBACK_DEBUG_
   uint8_t destip[4];
   uint16_t destport;
#endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
		 printf("sock established\r\n");
         if(getSn_IR(sn) & Sn_IR_CON)
         {
#ifdef _LOOPBACK_DEBUG_
			getSn_DIPR(sn, destip);
			destport = getSn_DPORT(sn);

			printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn,Sn_IR_CON);
         }
		 
		 ret = send(sn, buf, len);
		 if(ret < 0){			 
			close(sn);
			return sn;
		 }
		 
		 break;
      case SOCK_CLOSE_WAIT :
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:CloseWait\r\n",sn);
#endif
         if((ret = disconnect(sn)) != SOCK_OK) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;
      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	 printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:TCP server loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_TCP, Port, 0x00)) != sn) return ret;
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}

uint32_t net_recvData(uint8_t sn, uint8_t *buf, uint16_t len){
	int32_t ret;
	uint16_t size = 0, sentsize=0;

#ifdef _LOOPBACK_DEBUG_
   uint8_t destip[4];
   uint16_t destport;
#endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
		 printf("sock established\r\n");
         if(getSn_IR(sn) & Sn_IR_CON)
         {
#ifdef _LOOPBACK_DEBUG_
			getSn_DIPR(sn, destip);
			destport = getSn_DPORT(sn);

			printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn,Sn_IR_CON);
         }
		 if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
         {
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
			ret = recv(sn, buf, size);

			if(ret <= 0) 
				return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
         }
         break;
      case SOCK_CLOSE_WAIT :
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:CloseWait\r\n",sn);
#endif
         if((ret = disconnect(sn)) != SOCK_OK) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;
      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	 printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:TCP server loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_TCP, Port, 0x00)) != sn) return ret;
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}