/*
 * tim_interrupt.c
 *
 *  Created on: 28 lis 2017
 *      Author: wiciu
 */

#include "stm32f10x.h"
#include "variables.h"
#include "tim_interrupt.h"
#include "pid.h"

//przerwanie od przepelnienia timera - co 1ms, do zczytywania tykniec enkoderow
void TIM4_IRQHandler() {
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

		pulse_count1 = pulse_count2;
		pulse_count2 = TIM_GetCounter(TIM3);	//zczytanie stanu enkodera

		//dla przypadku gdy s¹ ró¿ne kierunki obrotu silnika
		if(driving_mode[1]==1 || driving_mode[3]==1 || driving_mode[5]==1 || driving_mode[6]==1){
			if(pulse_count2 >= pulse_count1)		e = pulse_count2 - pulse_count1;
			else									e = (1200 - pulse_count1) + pulse_count2;
		}

		if(driving_mode[0]==1 || driving_mode[2]==1 || driving_mode[4]==1 || driving_mode[6]==2){
			if(pulse_count1 >= pulse_count2)		e = pulse_count1 - pulse_count2;
			else									e = (1200 - pulse_count2) + pulse_count1;
		}

		enc_value=e;									//zczytana wartoœc
		pid_value=pid_calculate(set_value, enc_value);	//obliczenie PWM na silnik

		if(i<400){
			enc_value_diff[i]=e;	//do wyznaczenia ch-ki rozruchu
			i++;
		}
	}
}
