/*
 * bluetooth.h
 *
 *  Created on: Jun 7, 2015
 *      Author: psampaio
 */

#ifndef TESTBT_INC_BLUETOOTH_H_
#define TESTBT_INC_BLUETOOTH_H_

#define UART_ID		LPC_UART1

void BLUETOOTH_Init(uint32_t baudRate);
int BLUETOOTH_State(uint8_t *buffer, uint32_t lenght);
int BLUETOOTH_Read(uint8_t *buffer, uint32_t lenght);
int BLUETOOTH_Disconnect(uint8_t *buffer, uint32_t lenght);
int BLUETOOTH_Connect(uint8_t *buffer, uint32_t lenght);
int BLUETOOTH_Scan(uint8_t *buffer, uint32_t lenght);

int IsConnected(uint8_t *buffer);
int IsDisconnected(uint8_t *buffer);

#endif /* TESTBT_INC_BLUETOOTH_H_ */
