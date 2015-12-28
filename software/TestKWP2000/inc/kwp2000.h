/*
 * kwp2000.h
 *
 *  Created on: Jul 23, 2015
 *      Author: psampaio
 */

#ifndef KWP2000_H_
#define KWP2000_H_

#define UART_ID		((LPC_UART_TypeDef *)LPC_UART0)

#define KWP2000_SCANTOOL_ADDR 0xF1
#define KWP2000_ECU_ADDR 0x33

#define KWP2000_GET_LENGTH(_x)  ((_x)&0x3F)

#define KWP2000_LENGTH_BYTE( len )  (0xC0 | len)
#define KWP2000_PHY_LENGTH_BYTE( len )  (0x80 | len)

#define KWP2000_NOT_CONNECTED 10
#define KWP2000_CONNECTED     11

#define KWP2000_CRC_ERROR     12
#define KWP2000_ECU_ERROR     13
#define KWP2000_RX_BUF_EMPTY  14
#define KWP2000_MSG_INCOMPLETED 15

#define KWP2000_OK            1
#define KWP2000_FAILED        0

int KWP2000SlowInit(void);
int KWP2000FastInit(void);
int KWP2000SendPacket(uint8_t dst_addr, uint8_t src_addr,uint8_t *,uint8_t len);
int KWP2000SendRawPacket(uint8_t *,uint8_t len);

void kwp2000_set_connection_status( uint8_t );

uint8_t kwp2000_connection_status();
uint8_t KWP2000CheckConnectionStatus(void);

uint8_t KWP2000CheckMsg(uint8_t *buffer, uint8_t * len);
//uint8_t * kwp2000_data_buf();


#endif /* KWP2000_H_ */
