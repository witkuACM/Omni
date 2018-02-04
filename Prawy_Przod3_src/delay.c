/*
 * delay.c
 *
 *  Created on: 11 lis 2017
 *      Author: wiciu
 */

#include "delay.h"
#include "variables.h"

volatile static uint32_t timer_ms = 0;

void SysTick_Handler()
{
 if (timer_ms)
 timer_ms--;
}

void delay_ms(int time)
{
 timer_ms = time;
 while (timer_ms);
}
