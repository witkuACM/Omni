/*
 * exti_interrupt.c
 *
 *  Created on: 14 lis 2017
 *      Author: wiciu
 */

#include "stm32f10x.h"
#include "exti_interrupt.h"
#include "variables.h"
#include "uart_interrupt.h"

//CZUJNIKI - PRZERWANIA ZEWNETRZNE OD CZUJNIKOW CYFROWYCH
void EXTI3_IRQHandler()
{
	//send_char1('1');
	//blok_tyl = 1;
	EXTI_ClearITPendingBit(EXTI_Line3);
}
void EXTI4_IRQHandler()
{
	//send_char1('2');
	//blok_tyl = 1;
	EXTI_ClearITPendingBit(EXTI_Line4);
}
void EXTI9_5_IRQHandler()
{
	//send_char1('3');
	//blok_tyl=1;
	EXTI_ClearITPendingBit(EXTI_Line7);
	EXTI_ClearITPendingBit(EXTI_Line9);
}
void EXTI15_10_IRQHandler()//dziala
{
	//send_char1('4');
	//blok_przod=1;
	//EXTI_ClearITPendingBit(EXTI_Line13);
	EXTI_ClearITPendingBit(EXTI_Line14);
	EXTI_ClearITPendingBit(EXTI_Line15);
}
