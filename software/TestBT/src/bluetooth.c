/*
 * bluetooth.c
 *
 *  Created on: Jun 7, 2015
 *      Author: psampaio
 */

#include <string.h>
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "bluetooth.h"

uint8_t atQuery[]  = "AT\r";
uint8_t atInfo[]   = "AT+BTINFO?\r";
uint8_t atInfo1[]   = "AT+BTINFO?1\r";
uint8_t atScan[]   = "AT+BTSCAN\r";
uint8_t atReset[]  = "ATZ\r";
uint8_t atCancel[] = "AT+BTCANCEL\r";

void SendATCommand(unsigned char * cmd) {
	UART_Send(UART_ID, cmd, strlen((char*)cmd), BLOCKING);
}
#if 0
void send_command2(unsigned char * cmd, int size) {
	UART_Send(UART_ID, cmd, size, BLOCKING);
}

int valid_response(int numRetry) {
	char aux = 0;
	uint8_t lsr;

	while (aux != '\n' && numRetry > 0) {
		lsr = UART_GetLineStatus(UART_ID);
		if ((lsr & 0x01) == 0) {
			numRetry--;
			continue;
		}
		UART_ReceiveByte(UART_ID);
		return UART_ReceiveByte(UART_ID);
	}

	return (numRetry <= 0) ? 0: 1;
}


int read_command_response(uint8_t *buffer) {
	int result = -1;
	if(valid_response(1000000)) {
		result = UART_Receive(UART_ID, buffer, 20, NONE_BLOCKING);
		if ((buffer[result-1] == '\n') && (buffer[result-1] == '\r')) {
			buffer[result-2] = 0;
			result -= 2;
		}
	}
	return result;
}
#endif

int ReceiveATAnswer(uint8_t *buffer, uint32_t lenght) {
	uint8_t lsr;
	int res = 0;
	int state = 0;

	while(1) {
		uint32_t start = SYSTICK_GetTicks();
		lsr = UART_GetLineStatus(UART_ID);
		while ((lsr & 0x01) == 0) {
			if (SYSTICK_GetTicks() > start + 5000) return 0;
			lsr = UART_GetLineStatus(UART_ID);
		}
		res = UART_ReceiveByte(UART_ID);
		if (res == '\r' && state == 0) {
			state = 1;
			continue;
		}
		if (res == '\n' && state == 1) {
			break;
		}
		else {
			state = 0;
		}
	}
	int i = 0;
	do {
		while ((UART_GetLineStatus(UART_ID) & 0x01) == 0);
		res = UART_ReceiveByte(UART_ID);
		if (res == '\r') {
			while ((UART_GetLineStatus(UART_ID) & 0x01) == 0);
			res = UART_ReceiveByte(UART_ID);
			if (res == '\n') {
				buffer[i] = 0;
				//printf("%s\n", buffer);
				return i;
			}
		}
		if (i < lenght)	buffer[i++] = res;
	} while(1);
}

int CommandOrDataAns(uint8_t *buffer, uint32_t lenght) {
	uint8_t lsr;
	int res = 0;
	int state = 0;

	while(1) {
		uint32_t start = SYSTICK_GetTicks();
		lsr = UART_GetLineStatus(UART_ID);
		while ((lsr & 0x01) == 0) {
			if (SYSTICK_GetTicks() > start + 5000) return 0;
			lsr = UART_GetLineStatus(UART_ID);
		}
		res = UART_ReceiveByte(UART_ID);
		if (res == '\r' && state == 0) {
			state = 1;
			continue;
		}
		if (res == '\n' && state == 1) {
			break;
		}
		else {
			state = 2;
			break;
		}
	}
	int i = 0;
	if (state == 1) {
		do {
			while ((UART_GetLineStatus(UART_ID) & 0x01) == 0);
			res = UART_ReceiveByte(UART_ID);
			if (res == '\r') {
				while ((UART_GetLineStatus(UART_ID) & 0x01) == 0);
				res = UART_ReceiveByte(UART_ID);
				if (res == '\n') {
					buffer[i] = 0;
					printf("%s\n", buffer);
					return i;
				}
			}
			if (i < lenght)	buffer[i++] = res;
		} while(1);
	}
	if (state == 2) {
		buffer[0] = res;
		i = UART_Receive(UART_ID, &buffer[1], 30, NONE_BLOCKING) + 1;
		return i;
	}
	return 0;
}


/*
 *
 int read_response(uint8_t *buffer) {
	int res = read_command_response(buffer);
	if (res != -1) {
		buffer[res] = 0;
	}
	return res;
}
*/

int BLUETOOTH_Read(uint8_t *buffer, uint32_t lenght)
{
	return CommandOrDataAns(buffer, lenght);
}
int BLUETOOTH_Readx(uint8_t *buffer, uint32_t lenght)
{
	int len = -1;
	char aux;
	uint8_t lsr;

	uint32_t start = SYSTICK_GetTicks();
	lsr = UART_GetLineStatus(UART_ID);
	while ((lsr & 0x01) == 0) {
		if (SYSTICK_GetTicks() > start + 3000) return 0;
		lsr = UART_GetLineStatus(UART_ID);
	}
	aux = UART_ReceiveByte(UART_ID);
	if((aux == '\r')) {// response command
		UART_Receive(UART_ID, buffer, 1, NONE_BLOCKING);	// consumir o '\n'
		len = UART_Receive(UART_ID, buffer, 20, NONE_BLOCKING);
		if (len < 2) return 0;
		if (buffer[len - 1] = '\n') {
			buffer[len - 2] = 0;
			return len - 2;
		}
	}
	else {
		buffer[0] = aux;
		len = UART_Receive(UART_ID, &buffer[1], 20, NONE_BLOCKING);
		len++;
		buffer[len] = 0;
		return len;
	}
	return 0;
}

static char* appendString(char* cmd, char* str2){
	int i=0;
	int j;

	while(cmd[i]!='\0'){
		++i;
	}

	j=i;
	while(j!=(i+12) && *str2){
		cmd[j]=*str2;
		++j;
		++str2;
	}

	cmd[j]=0x0d;
	++j;
	cmd[j]='\0';

	return cmd;

}

int IsConnected(uint8_t *buffer)
{
	return strstr((char *)buffer, "CONNECT") != 0;
}

int IsDisconnected(uint8_t *buffer)
{
	return strcmp((char *)buffer, "DISCONNECT") == 0;
}

int BLUETOOTH_Disconnect(uint8_t *buffer, uint32_t lenght)
{
	SendATCommand(atCancel);
	return ReceiveATAnswer(buffer, lenght);
}

int BLUETOOTH_Connect(uint8_t *buffer, uint32_t lenght)
{
	SendATCommand(atQuery);
	return ReceiveATAnswer(buffer, lenght);
}

int BLUETOOTH_Info(uint8_t *buffer, uint32_t lenght) {
	SendATCommand(atInfo);
	return ReceiveATAnswer(buffer, lenght);
}

int BLUETOOTH_Info1(uint8_t *buffer, uint32_t lenght) {
	SendATCommand(atInfo1);
	return ReceiveATAnswer(buffer, lenght);
}

int BLUETOOTH_Scan(uint8_t *buffer, uint32_t lenght)
{
	SendATCommand(atScan);
	return ReceiveATAnswer(buffer, lenght);
}

int BLUETOOTH_Reset(uint8_t *buffer, uint32_t lenght)
{
	SendATCommand(atReset);
	return ReceiveATAnswer(buffer, lenght);
}



void bluetooth_setName(char * name, uint8_t *buffer){
	char cmd_Name[24]="AT+BTNAME=";
	char *str;
	str = appendString(cmd_Name, name);
	SendATCommand(str);//envio do comando completo
	read_command_response(buffer);
}

void bluetooth_setKey(char * key, uint8_t *buffer){
	char cmd_Key[23]="AT+BTKEY=";
	char *str;
	str = appendString(cmd_Key, key);
	SendAtCommand(str);	//envio do comando completo
	read_command_response(buffer);
}

void UartInitialize(unsigned int baud) {
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize UART1 pin connect
	 * set pins:  port2.0 -> TX1,  port2.1 -> RX1
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
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

void BLUETOOTH_Init(uint32_t baudRate)
{
	uint8_t buffer[30];
	int retries = 1;

    UartInitialize(baudRate);
    UartRxFlush();

    do {
    	memset(buffer, 0,sizeof(buffer));
    	BLUETOOTH_Connect(buffer, 30);
        printf("Connect: %s\n", buffer);
    } while (strcmp((char *)buffer, "OK") != 0);

	BLUETOOTH_Info1(buffer, 30);
    printf("Info1: %s\n", buffer);

    BLUETOOTH_Info(buffer, 30);
    printf("Info: %s\n", buffer);

	if(strcmp((char *)buffer, "STANDBY") != 0) {
		BLUETOOTH_Disconnect(buffer, 30);
		printf("Not in STANDBY. Disconnect: %s\n", buffer);
	}

	BLUETOOTH_Connect(buffer, 30);
    printf("Connect: %s\n", buffer);

	while((strcmp((char *)buffer,"OK") != 0) && retries>0){
		BLUETOOTH_Connect(buffer, 30);
	    printf("Retry Connect: %s\n", buffer);
		retries--;
	}

	if(strcmp((char *)buffer,"OK") == 0) {
		BLUETOOTH_Scan(buffer, 30);
		printf("Scan: %s\n", buffer);
	}
}
