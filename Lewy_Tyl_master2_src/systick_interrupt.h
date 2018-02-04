/*
 * delay.h
 *
 *  Created on: 11 lis 2017
 *      Author: wiciu
 */

#ifndef SYSTICK_INTERRUPT_H_
#define SYSTICK_INTERRUPT_H_

extern void SysTick_Handler();
extern void delay_ms(int time);
extern unsigned long int SysTick_Config_Mod(unsigned long int SysTick_CLKSource, unsigned long int Ticks);

#endif /* SYSTICK_INTERRUPT_H_ */
