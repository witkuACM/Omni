/*
 * delay.c
 *
 *  Created on: 11 lis 2017
 *      Author: wiciu
 */

#include "stm32f10x.h"
#include "systick_interrupt.h"
#include "variables.h"
#include "uart_interrupt.h"

volatile static uint32_t timer_ms = 0;

void SysTick_Handler()
{
	if (timer_ms)			//odmierzanie czasu opóŸnienia - z funkcji Delay
		timer_ms--;

	//CZUJNIKI - STANY
 	if (/*(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 1)&&*/(adc_value[lbnad]<vmax_reszta)&&(adc_value[lfnad]<vmax_reszta))
 		blok_lewo=0;
 	else	blok_lewo=1;

 	if (/*(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 1)&&*/(adc_value[rbnad]<vmax_reszta)&&(adc_value[rfnad]<vmax_reszta))
 		blok_prawo=0;
 	else	blok_prawo=1;

 	if ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1)&&(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 1))
 		blok_tyl=0;
 	else	blok_tyl=1;

 	if ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 1)&&(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 1)&&(adc_value[rf]<vmax_przod)&&(adc_value[lf]<vmax_przod))
 		blok_przod=0;
 	else	blok_przod=1;
}

/*	uint16_t ba=adc_value[lbnad];
	uint16_t ca=adc_value[lfnad];
	uint16_t da=adc_value[lf];
	uint16_t ga=adc_value[rf];
	uint16_t fa=adc_value[rfnad];
	uint16_t ra=adc_value[rbnad];
	if(ba>2000)
		send_char1('1');
	if(ca>2000)
		send_char1('6');
	if(da>2000)
		send_char1('3');
	if(ga>2000)
		send_char1('2');
	if(fa>2000)
		send_char1 ('5');
	if(ra>2000)
		send_char1('4');*/

void delay_ms(int time)
{
	timer_ms = time;
	while (timer_ms);
}

unsigned long int SysTick_Config_Mod(unsigned long int SysTick_CLKSource, unsigned long int Ticks)
{
	//inicjalizacja licznika SysTick
	//zastepuje funkcje z bibliotek STM w zwiazku z bledem w funcji SysTick_Config
	unsigned long int Settings;

	assert_param(IS_SYSTICK_CLK_SOURCE(SysTick_CLKSource));

	if (Ticks > SysTick_LOAD_RELOAD_Msk)  return (1);             //Kontrola, czy wartosc poczatkowa nie przekracza max

	SysTick->LOAD = (Ticks & SysTick_LOAD_RELOAD_Msk) - 1;        //Ustaw wartosc poczatkowa licznika
	NVIC_SetPriority (SysTick_IRQn, 0);                           //Ustaw priorytet przerwania
	SysTick->VAL  = 0;                                            //Ustaw wartosc aktualna licznika
	Settings=SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;  //Ustaw flagi wlaczaenia SysTick IRQ  i samego licznika
	if (SysTick_CLKSource == SysTick_CLKSource_HCLK){             //Wybierz flage ustawien zrodla sygnalu zegarowego
		Settings |= SysTick_CLKSource_HCLK;
	} 	else {
		Settings &= SysTick_CLKSource_HCLK_Div8;
	}
	SysTick->CTRL = Settings;                                     //Zapisz ustawienia do rejestru sterujacego SysTick (i wlacz licznik)
	return (0);
}
