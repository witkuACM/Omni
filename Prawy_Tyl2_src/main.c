/*
 * WERSJA DRUGA
 * Program SLAVE PRAWY TYL
 * Sterowanie W-S-A-D oraz spacja
 * Rozszerzone o regulator PID
*/

#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "configuration.h"
#include "delay.h"
#include "driving_functions.h"
#include "frame.h"
#include "uart_interrupt.h"
#include "pid.h"
#include "tim_interrupt.h"
#include "steerage.h"
#include "variables.h"


int main(void)
{
	SysTick_Config(SystemCoreClock / 1000);

	RCCinit();
	GPIOinit();
	DMAinit();
	NVICinit();
	UARTinit();
	TIMinit();

	ramka(tab);

	while(1){
		sterowanie(tab_dma);
	}
}
