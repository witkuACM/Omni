/*
 * uart_interrupt.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#include "stm32f10x.h"
#include "uart_interrupt.h"
#include "variables.h"

void send_char3(char c)	//procedura wyslania znaku przez UART3 - ze Slave do Mastera; aktualnie nie uzywana
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	//czekamy az bufor nadawczy bedzie wolny
    USART_SendData(USART3, c);
}

void USART1_IRQHandler()		//przerwanie od odbierania danych przez UART3
{
		//PODCZAS CIRCULARA DMA FLAGA OD ODEBRANIA DANYCH WYZEROWANA - rejestr DR ciagle sprawdzany
		 char c = USART_ReceiveData(USART1);
		 tab[START_SIZE]=c;
		 sprawdz_ramke(tab);
		 czysc_dane(tab);
}
