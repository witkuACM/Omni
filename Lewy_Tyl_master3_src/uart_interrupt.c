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

void send_char1(char c)			//procedura wyslania znaku przez UART1 TEST CZUJNIKOW ODLEGLOSCI
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//czekamy az bufor nadawczy uart1 bedzie wolny
    	USART_SendData(USART1, c);
}

void send_char3(char c)			//procedura wyslania znaku przez UART3 - z Mastera do Slave
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	//czekamy az bufor nadawczy bedzie wolny
    	USART_SendData(USART3, c);
}

void USART1_IRQHandler()		//przerwanie od odbierania danych przez UART1
{
	char c = USART_ReceiveData(USART1);//odebrana dana
	tab[START_SIZE]=c;					//umieszczenie danej w ramce UART
	sprawdz_ramke(tab);				//sprawdzenie poprawnoœci ramki (tu wystawione flagi)

	if(c=='#'){
		k=0;
	}

	start_instrukcji();
	sprawdz_ramke_xy(tab_instrukcje);
	k++;

	if(k==INSTR_SIZE){
		k=0;
		FLAG_END_OF_INSTR=true;
	}
	else{
		FLAG_END_OF_INSTR=false;
	}

	czysc_dane(tab);					//czyszczenie bufora danych
	send_char1(c);						//wys³ana dalej do slave'ów
}



//PODCZAS CIRCULARA DMA FLAGA OD ODEBRANIA DANYCH WYZEROWANA - rejestr DR ciagle sprawdzany
