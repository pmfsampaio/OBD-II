/*
===============================================================================
 Name        : TestBT.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <string.h>
#include <stdio.h>
#include "bluetooth.h"

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#define USER_LED_MASK	(1 << 22)

// Function to initialize GPIO to access the user LED @ P0_22
void LED_Init(void)
{
	// Set P0_22 to 00 - GPIO
	LPC_PINCON->PINSEL1	&= (~0x00003000);
	// Set GPIO - P0_22 - to be outputs
	LPC_GPIO0->FIOSET = USER_LED_MASK;
	LPC_GPIO0->FIODIR |= USER_LED_MASK;
}

// Function to turn user LED on
void LED_On(void)
{
	LPC_GPIO0->FIOCLR = USER_LED_MASK;
}

// Function to turn user LED off
void LED_Off(void)
{
	LPC_GPIO0->FIOSET = USER_LED_MASK;
}

volatile uint32_t msTicks; // counter for 1ms SysTicks

//  SysTick_Handler - just increment SysTick counter
void SysTick_Handler(void) {
  msTicks++;
}

// DelayMS - creates a delay of the appropriate number of Systicks (happens every 1 ms)
void DelayMS (uint32_t delayTicks) {
  uint32_t currentTicks;

  currentTicks = msTicks;	// read current tick counter
  // Now loop until required number of ticks passes.
  while ((msTicks - currentTicks) < delayTicks);
}

uint32_t SYSTICK_GetTicks(void)
{
	return msTicks;
}

void SYSTICK_Delay(uint32_t delayTicks) {
  uint32_t currentTicks;

  currentTicks = msTicks;	// read current tick counter
  // Now loop until required number of ticks passes.
  while ((msTicks - currentTicks) < delayTicks);
}

int state;
int echoOn = 1;
int lineFeedOn = 1;
int headerOn = 0;

int main(void) {

	uint8_t s[20], command[50];
	int res, posCurr;

	printf("Test Bluetooth\n");

	// Setup SysTick Timer to interrupt at 1 msec intervals
	if (SysTick_Config(SystemCoreClock / 1000)) {
	    while (1);  // Capture error
	}

//    LED_Init();

    BLUETOOTH_Init(9600);
    state = 0;
    posCurr = 0;

    while(1) {
    	if (state) {
    		res = BLUETOOTH_Read(s, 30);
    		if (res == 0) continue;
    		if (IsDisconnected(s)) {
    			printf("State: Disconnect...\n");
    			state = 0;
    		}
    		else {
    			memcpy(&command[posCurr], s, res);
    			posCurr += res;
    			if (command[posCurr - 1] == '\r') {
        			command[posCurr] = 0;
        			printf("%s\n", command);
        			posCurr = 0;
        			if (strcmp(command, "ATZ\r") == 0) {
        				SendATCommand("ELM327 v2.1\r");
        			}
        			if (strcmp(command, "ATI\r") == 0) {
        				SendATCommand("ELM327 v2.1\r");
        			}
        			if (strcmp(command, "AT@1\r") == 0) {
        				SendATCommand("OBDII to RS232 Interpreter\r");
        			}
        			if (strcmp(command, "AT@2\r") == 0) {
        				SendATCommand("?\r");
        			}
        			if (strcmp(command, "ATE0\r") == 0) {
        				echoOn = 0;
        				SendATCommand("ATE0 OK\r");
        			}
        			if (strcmp(command, "ATL0\r") == 0) {
        				lineFeedOn = 0;
        				SendATCommand("OK\r");
        			}
        			if (strcmp(command, "ATS0\r") == 0) {
        				SendATCommand("OK\r");
        			}
        			if (strcmp(command, "ATM0\r") == 0) {
        				SendATCommand("OK\r");
        			}
        			if (strcmp(command, "ATTP5\r") == 0) {
        				SendATCommand("OK\r");
        			}
        			if (strcmp(command, "ATSP5\r") == 0) {
        				SendATCommand("OK\r");
        			}
        			if (strcmp(command, "0100\r") == 0) {
        				if (headerOn) {
        					SendATCommand("41 6B 10 41 00 BF DF B9 91 78\r");
        				}
        				else {
        					SendATCommand("41 00 BF DF B9 91\r");
        				}
        			}
        			if (strcmp(command, "ATDPN\r") == 0) {
        				SendATCommand("5\r");
        			}
        			if (strcmp(command, "ATDP\r") == 0) {
        				SendATCommand("ISO14230-4(KWPFAST)\r");
        			}
        			if (strcmp(command, "ATH1\r") == 0) {
        				headerOn = 1;
        				SendATCommand("OK\r");
        			}


        			SendATCommand(">\r");
    			}
//    			s[res] = 0;
    			if (s[0] == 'q') LED_On();
    			if (s[0] == 'a') LED_Off();
//    			printf("%s\n", s);
    		}
    	}
    	else {
    		res = BLUETOOTH_Read(s, 30);
    		if (res == 0) continue;
    		if (IsConnected(s)) {
    			printf("State: Connect...\n");
    			state = 1;
    		}
    	}
    	DelayMS(10);
    }


    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}
