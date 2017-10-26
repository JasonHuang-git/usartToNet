#ifndef _NET_H
#define _NET_H

#define DATA_BUF_SIZE			2048

static void setNetWorkDefault(void);
static void phy_init(void);
static void setKeepAlive(SOCKET s);
static uint32_t server_Init(uint8_t sn, int port);

extern void net_Init(void);
extern int32_t net_SendData(uint8_t sn, uint8_t* buf, uint16_t len);
extern int32_t net_recvData(uint8_t sn, uint8_t *buf);

#endif
