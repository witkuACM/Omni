/*
 * driving_functions.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#include "stm32f10x.h"
#include "driving_functions.h"
#include "variables.h"
#include "pid.h"

/*
 * driving_mode[0]-przod		- 	   W
 * driving_mode[1]-tyl			-	   S
 * driving_mode[2]-lewo			-	   A
 * driving_mode[3]-prawo		-	   D
 * driving_mode[4]-zwrotLewo	-	   Q
 * driving_mode[5]-zwrotPrawo	-      E
 * driving_mode[6]-stop			-	 SPACJA
*/

void przod(uint16_t v)
{
	if(driving_mode[0]==0 || zmiana_stanu==true){	i=0;				//ustawianie kierunku jazdy
		driving_mode[0]=1;driving_mode[1]=0;driving_mode[2]=0;driving_mode[3]=0;
		driving_mode[4]=0;driving_mode[5]=0;driving_mode[6]=0;

		//wy³¹czenie mostka silnikowego przy zmianie kierunku jazdy
		set_value_obrot_min = 0;
		TIM_SetCompare4(TIM2, pid_value);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);

		//ustawienie pinów kierunkowych mostka
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
		set_value_obrot_min=v;				//wartoœc zadana = 500obr/min - prêdkosc znamionowa
		zamiana_jednostek(set_value_obrot_min);	//zamiana jednostek na impulsy/ms
	}
	TIM_SetCompare4(TIM2, pid_value);			//ustawienie wype³nienia PWM na silnik
}
void tyl(uint16_t v)
{
	if(driving_mode[1]==0 || zmiana_stanu==true){	i=0;
		driving_mode[1]=1;driving_mode[0]=0;driving_mode[2]=0;driving_mode[3]=0;driving_mode[4]=0;driving_mode[5]=0;driving_mode[6]=0;
		set_value_obrot_min = 0;
		TIM_SetCompare4(TIM2, pid_value);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(GPIOC, GPIO_Pin_7);
		set_value_obrot_min=v;
		zamiana_jednostek(set_value_obrot_min);
	}
	TIM_SetCompare4(TIM2, pid_value);
}
void lewo(uint16_t v)
{
	if(driving_mode[2]==0 || zmiana_stanu==true){	i=0;
		driving_mode[2]=1;driving_mode[0]=0;driving_mode[1]=0;driving_mode[3]=0;driving_mode[4]=0;driving_mode[5]=0;driving_mode[6]=0;
		set_value_obrot_min = 0;
		TIM_SetCompare4(TIM2, pid_value);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(GPIOC, GPIO_Pin_7);
		set_value_obrot_min=v;
		zamiana_jednostek(set_value_obrot_min);
	}
	TIM_SetCompare4(TIM2, pid_value);
}
void prawo(uint16_t v)
{
	if(driving_mode[3]==0 || zmiana_stanu==true){	i=0;
		driving_mode[3]=1;driving_mode[0]=0;driving_mode[1]=0;driving_mode[2]=0;driving_mode[4]=0;driving_mode[5]=0;driving_mode[6]=0;
		set_value_obrot_min = 0;
		TIM_SetCompare4(TIM2, pid_value);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
		set_value_obrot_min = v;
		zamiana_jednostek(set_value_obrot_min);
	}
	TIM_SetCompare4(TIM2, pid_value);
}
void zwrotLewo()
{
	if(driving_mode[4]==0){	i=0;
		driving_mode[4]++;driving_mode[0]=0;driving_mode[1]=0;driving_mode[2]=0;driving_mode[3]=0;driving_mode[5]=0;driving_mode[6]=0;
		set_value_obrot_min = 0;
		TIM_SetCompare4(TIM2, pid_value);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
		set_value_obrot_min = 500;
		zamiana_jednostek(set_value_obrot_min);
	}
	TIM_SetCompare4(TIM2, pid_value);
}
void zwrotPrawo()
{
	if(driving_mode[5]==0){	i=0;
		driving_mode[5]++;driving_mode[0]=0;driving_mode[1]=0;driving_mode[2]=0;driving_mode[3]=0;driving_mode[4]=0;driving_mode[6]=0;
		set_value_obrot_min = 0;
		TIM_SetCompare4(TIM2, pid_value);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(GPIOC, GPIO_Pin_7);
		set_value_obrot_min = 500;
		zamiana_jednostek(set_value_obrot_min);
	}
	TIM_SetCompare4(TIM2, pid_value);
}
void stop()
{
	if(driving_mode[6]==0){	i=0;	//gdy zatrzymujemy sie to trzeba sprawdzic w ktora strone krecil sie wal, do skutecznego zatrzymania
		if(driving_mode[1]==1 || driving_mode[3]==1 || driving_mode[5]==1)		driving_mode[6]=1;
		else if(driving_mode[0]==1 || driving_mode[2]==1 || driving_mode[4]==1) driving_mode[6]=2;

		driving_mode[0]=0;driving_mode[1]=0;driving_mode[2]=0;driving_mode[3]=0;driving_mode[4]=0;driving_mode[5]=0;
		set_value_obrot_min = 0;
		zamiana_jednostek(set_value_obrot_min);
		TIM_SetCompare4(TIM2, pid_value);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	}
	TIM_SetCompare4(TIM2, pid_value);
}
void nic()
{
	TIM_SetCompare4(TIM2, pid_value);
}
