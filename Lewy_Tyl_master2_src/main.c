/*
 * WERSJA DRUGA
 * Program MASTER LEWY TYL
 * Sterowanie W-S-A-D oraz spacja
 * Rozszerzone o regulator PID i czujniki odleglosci
*/

#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "configuration.h"
#include "driving_functions.h"
#include "frame.h"
#include "uart_interrupt.h"
#include "pid.h"
#include "tim_interrupt.h"
#include "steerage.h"
#include "systick_interrupt.h"
#include "exti_interrupt.h"
#include "variables.h"

int main(void)
{
	SysTick_Config(SystemCoreClock / 1000);

	PeriphClock();	//CZUJNIKI
	RCCinit();
	GPIOinit();
	DMAinit();
	EXTIinit();		//CZUJNIKI
	NVICinit();
	UARTinit();
	TIMinit();
	ADCinit();  	//CZUJNIKI
	Multipleks();	//CZUJNIKI

	ramka(tab);

	while(1){
		sterowanie(tab_dma);
	}
}
