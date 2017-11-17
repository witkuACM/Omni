/*
 * driving_functions.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#include "stm32f10x.h"
#include "driving_functions.h"
#include "variables.h"

void przod()
{
	send_char3('w');
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void tyl()
{
	send_char3('s');
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void lewo()
{
	send_char3('a');
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void prawo()
{
	send_char3('d');
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void zwrotLewo()
{
	send_char3('q');
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void zwrotPrawo()
{
	send_char3('e');
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void stop()
{
	send_char3(' ');
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
}
void nic()
{
	switch(wypelnienie){
	default:
		TIM_SetCompare4(TIM2, wypelnienie);
		break;
	}
}
