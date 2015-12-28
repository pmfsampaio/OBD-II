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
#include "kwp2000.h"

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#define USER_LED_MASK	(1 << 22)

// Function to initialize GPIO to access the user LED @ P0_22
void LED_Init(void)
{
	// Set P0_22 to 00 - GPIO
//	LPC_PINCON->PINSEL1	&= (~0x00003000);
//	// Set GPIO - P0_22 - to be outputs
//	LPC_GPIO0->FIOSET = USER_LED_MASK;
//	LPC_GPIO0->FIODIR |= USER_LED_MASK;
}

// Function to turn user LED on
void LED_On(void)
{
//	LPC_GPIO0->FIOCLR = USER_LED_MASK;
}

// Function to turn user LED off
void LED_Off(void)
{
//	LPC_GPIO0->FIOSET = USER_LED_MASK;
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
uint8_t num_trying = 0;
uint8_t pidnum = 0;

int main(void) {
	int ix;
	uint8_t s[20], command[50], len;
	int res, posCurr;

	printf("Test KWP2000\n");

	// Setup SysTick Timer to interrupt at 1 msec intervals
	if (SysTick_Config(SystemCoreClock / 1000)) {
	    while (1);  // Capture error
	}

//    LED_Init();

    while(1) {
		if( kwp2000_connection_status() == KWP2000_NOT_CONNECTED )
		{
//			ISO9141Init();		// 55 e9 8f 70 cc
//			KWP2000SlowInit();	// 55 e9 8f 70 cc
			KWP2000FastInit();

			num_trying++;

		}
		else
		{
			uint8_t imit[]={0xc2, 0x33, 0xF1, 1, 0x0d};
			DelayMS(60);
			KWP2000SendRawPacket(imit, sizeof(imit));
			res = KWP2000CheckMsg(s, &len);
			UartRxFlush();
			printf("Speed: ");
			for (ix = 0; ix < len;ix++) {
				printf("%02x ", s[ix]);
			}
			printf("\n");

		}
    }


    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}
