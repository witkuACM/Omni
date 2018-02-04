/*
 * uart_interrupt.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#include "stm32f10x.h"
#include "uart_interrupt.h"
#include "variables.h"
#include "frame.h"

void USART3_IRQHandler()		//przerwanie od odbierania danych przez UART3
{
	//PODCZAS CIRCULARA DMA FLAGA OD ODEBRANIA DANYCH WYZEROWANA - rejestr DR ciagle sprawdzany
	 char c = USART_ReceiveData(USART3);
	 tab[START_SIZE]=c;
	 sprawdz_ramke(tab);
	 czysc_dane(tab);
}
