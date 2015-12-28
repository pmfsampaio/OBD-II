/*
 * kwp2000.c
 *
 *  Created on: Jul 23, 2015
 *      Author: psampaio
 */

#include <string.h>
#include <stdio.h>

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"

#include "kwp2000.h"

//#define DEBUG_MSG 1


volatile static uint8_t g_kwp2000_connection_status = KWP2000_NOT_CONNECTED;


void DelayMS (uint32_t delayTicks);
uint32_t SYSTICK_GetTicks(void);

#define TX_SET()  	LPC_GPIO0->FIOSET = (1 << 2)
#define TX_CLEAR() LPC_GPIO0->FIOCLR = (1 << 2)

void UartInitialize(unsigned int baud) {
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize UART0 pin connect
	 * set pins:  port0.2 -> TX0,  port0.3 -> RX0
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 2;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 115200bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);

	UARTConfigStruct.Baud_rate = baud;

	// Initialize UART2 peripheral with given to corresponding parameter
	UART_Init(UART_ID, &UARTConfigStruct);

	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Initialize FIFO for UART2 peripheral
	UART_FIFOConfig(UART_ID, &UARTFIFOConfigStruct);

	// Enable UART Transmit
	UART_TxCmd(UART_ID, ENABLE);
}

void UartRxFlush(void)
{
	while ((UART_GetLineStatus(UART_ID) & 0x01) != 0) {
		UART_ReceiveByte(UART_ID);
	}

}

void UartDisable(void)
{
	PINSEL_CFG_Type PinCfg;

//	UART_DeInit(UART_ID);

	/*
	 * Initialize UART2 pin connect
	 * set pins:  port0.2 -> TX0,  port0.3 -> RX0
	 */
	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 2;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);

	LPC_GPIO0->FIOSET = (1 << 2);
	LPC_GPIO0->FIODIR |= (1 << 2);
}

int ISO9141Init(void)
{
	uint8_t buffer[30];
	uint8_t i;

	UartDisable();

	DelayMS(300);
	TX_CLEAR();
	DelayMS(200);	// start

	TX_SET();
	DelayMS(400);	// 0 - 1

	TX_CLEAR();
	DelayMS(400);	// 2 - 3

	TX_SET();
	DelayMS(400);	// 4 - 5

	TX_CLEAR();
	DelayMS(400);	// 6 - 7

	TX_SET();
	DelayMS(250);	// stop + idle

	UartInitialize(10400);
	UartRxFlush();

	DelayMS(100);
	buffer[0] = UART_ReceiveByte(UART_ID);
	DelayMS(5);
	buffer[1] = UART_ReceiveByte(UART_ID);
	buffer[2] = UART_ReceiveByte(UART_ID);

	UART_SendByte(UART_ID, ~buffer[2]);

	DelayMS(300);
	buffer[3] = UART_ReceiveByte(UART_ID);
	buffer[4] = UART_ReceiveByte(UART_ID);

#ifdef DEBUG_MSG
	printf("ISO9141Init: ");
	for (i = 0; i < 5;i++) {
		printf("%02x ", buffer[i]);
	}
	printf("\n");
#endif

	return 0;
}


int KWP2000SlowInit(void)
{
	uint8_t buffer[30];
	uint8_t i;

	UartDisable();

	DelayMS(300);
	TX_CLEAR();
	DelayMS(200);	// start

	TX_SET();
	DelayMS(400);	// 0 - 1

	TX_CLEAR();
	DelayMS(400);	// 2 - 3

	TX_SET();
	DelayMS(400);	// 4 - 5

	TX_CLEAR();
	DelayMS(400);	// 6 - 7

	TX_SET();
	DelayMS(250);	// stop + idle

	UartInitialize(10400);
	UartRxFlush();

	DelayMS(100);
	buffer[0] = UART_ReceiveByte(UART_ID);
	DelayMS(5);
	buffer[1] = UART_ReceiveByte(UART_ID);
	buffer[2] = UART_ReceiveByte(UART_ID);

	UART_SendByte(UART_ID, ~buffer[2]);

	DelayMS(300);
	buffer[3] = UART_ReceiveByte(UART_ID);
	buffer[4] = UART_ReceiveByte(UART_ID);

	if (buffer[4] == 0xCC) {
		g_kwp2000_connection_status= KWP2000_CONNECTED;
	}
	else return KWP2000_FAILED;

#ifdef DEBUG_MSG
	printf("SlowInit: ");
	for (i = 0; i < 5;i++) {
		printf("%02x ", buffer[i]);
	}
	printf("\n");
#endif

	return KWP2000_OK;
}


int KWP2000FastInit(void)
{
	uint8_t initc[] = {0x81};

	UartDisable();

	TX_SET();
	DelayMS(300);
	TX_CLEAR();
	DelayMS(24);
	TX_SET();
	DelayMS(24);

	UartInitialize(10400);
	UartRxFlush();

	KWP2000SendPacket( KWP2000_ECU_ADDR, KWP2000_SCANTOOL_ADDR, initc, 1 );

	return KWP2000CheckConnectionStatus();
}

int KWP2000SendRawPacket(uint8_t * _pkt, uint8_t len)
{
	int i;
	uint8_t buffer[16];
	int readed;
	uint8_t crc = 0;

	for(i=0;i<len;i++) {
		UART_SendByte(UART_ID, _pkt[i]);
		crc += _pkt[i];
		DelayMS(5);
	}
	UART_SendByte(UART_ID, crc);
	DelayMS(5);

	readed = UART_Receive(UART_ID, buffer, len + 1, BLOCKING);

#ifdef DEBUG_MSG
	printf("SendRawPacket: ");
	for (i = 0; i < readed; i++) {
		printf("%02x ", buffer[i]);
	}
	printf("\n");
#endif
	return 0;
}

int KWP2000SendPacket(uint8_t dst_addr, uint8_t src_addr, uint8_t * _pkt,uint8_t len)
{
	int i;
	uint8_t cmd[16];

	cmd[0] = KWP2000_LENGTH_BYTE(len);
	cmd[1] = dst_addr;
	cmd[2] = src_addr;

	for(i=0;i<len;i++)
	{
		cmd[i+3] = _pkt[i];
	}

	KWP2000SendRawPacket(cmd, len+3);

	return 0;
}


uint8_t kwp2000_connection_status()
{
	return g_kwp2000_connection_status;

}

uint8_t KWP2000CheckMsg(uint8_t *buffer, uint8_t * _len)
{
	uint8_t lsr, len;
	int i;


	*_len = 0;
	uint32_t start = SYSTICK_GetTicks();
	lsr = UART_GetLineStatus(UART_ID);
	while ((lsr & 0x01) == 0) {
		if (SYSTICK_GetTicks() > start + 5000) return KWP2000_RX_BUF_EMPTY;
		lsr = UART_GetLineStatus(UART_ID);
	}

	buffer[0] = UART_ReceiveByte(UART_ID);

	len = KWP2000_GET_LENGTH(buffer[0]) + 2 + 1;

	*_len = UART_Receive(UART_ID, &buffer[1], len, BLOCKING) + 1;

#ifdef DEBUG_MSG
	printf("CheckMsg: ");
	for (i = 0; i < *_len;i++) {
		printf("%02x ", buffer[i]);
	}
	printf("\n");
#endif

	if (*_len != (len + 1)) return KWP2000_MSG_INCOMPLETED;
	return KWP2000_OK;
}


uint8_t KWP2000CheckConnectionStatus(void)
{
	uint8_t buffer[30];
	uint8_t len;

	if( KWP2000CheckMsg(buffer, &len) == KWP2000_OK ) {
		if(buffer[0 + 3] == 0xC1) {
			g_kwp2000_connection_status= KWP2000_CONNECTED;
			return KWP2000_OK;
		}
	}
	return KWP2000_FAILED;
}

void kwp2000_set_connection_status( uint8_t _status)
{
	g_kwp2000_connection_status = _status;
}


