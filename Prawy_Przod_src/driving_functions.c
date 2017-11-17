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
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void tyl()
{
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void lewo()
{
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void prawo()
{
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void zwrotLewo()
{
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
