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

void USART3_IRQHandler()			//przerwanie od odbierania danych przez UART1 - bluetooth
{
	char c = USART_ReceiveData(USART3);
	tab[START_SIZE]=c;
	sprawdz_ramke(tab);

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

	czysc_dane(tab);
}
