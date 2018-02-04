/*
 * driving_functions.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#include "stm32f10x.h"
#include "driving_functions.h"
#include "variables.h"
#include "uart_interrupt.h"
#include "pid.h"

/*
 * driving_mode[0]-przod
 * driving_mode[1]-tyl
 * driving_mode[2]-lewo
 * driving_mode[3]-prawo
 * driving_mode[4]-zwrotLewo
 * driving_mode[5]-zwrotPrawo
 * driving_mode[6]-stop
*/

void przod(uint16_t v)	//NIECH v BEDZIE PREDKOSCIA OBROTOWA W OBR/MIN
{
	send_char3('#');send_char3('x');send_char3('<');send_char3(Vx);send_char3('>');send_char3('y');send_char3('<');send_char3(Vy);send_char3('>');
	if(driving_mode[0]==0 || zmiana_stanu==true){	i=0;				//ustawienie kierunku jazdy
		driving_mode[0]=1;driving_mode[1]=0;driving_mode[2]=0;driving_mode[3]=0;driving_mode[4]=0;driving_mode[5]=0;driving_mode[6]=0;
		set_value_obrot_min = 0;				//zerowanie stanu mostka H i silnika
		TIM_SetCompare4(TIM2, pid_value);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(GPIOC, GPIO_Pin_7);		//ustawienie kierunku jazdy
		set_value_obrot_min=v;					// i prêdkoœci = v obr/min
		zamiana_jednostek(set_value_obrot_min);	//zamiana na impulsy/milisekunde
	}
	TIM_SetCompare4(TIM2, pid_value);			//wystawienie PWM na silnik
}
void tyl(uint16_t v)
{
	send_char3('#');send_char3('x');send_char3('<');send_char3(Vx);send_char3('>');send_char3('y');send_char3('<');send_char3(Vy);send_char3('>');
	if(driving_mode[1]==0|| zmiana_stanu==true){	i=0;
		driving_mode[1]=1;driving_mode[0]=0;driving_mode[2]=0;driving_mode[3]=0;driving_mode[4]=0;driving_mode[5]=0;driving_mode[6]=0;
		set_value_obrot_min = 0;
		TIM_SetCompare4(TIM2, pid_value);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
		set_value_obrot_min=v;
		zamiana_jednostek(set_value_obrot_min);
	}
	TIM_SetCompare4(TIM2, pid_value);
}
void lewo(uint16_t v)
{
	send_char3('#');send_char3('x');send_char3('<');send_char3(Vx);send_char3('>');send_char3('y');send_char3('<');send_char3(Vy);send_char3('>');
	if(driving_mode[2]==0|| zmiana_stanu==true){	i=0;
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
	send_char3('#');send_char3('x');send_char3('<');send_char3(Vx);send_char3('>');send_char3('y');send_char3('<');send_char3(Vy);send_char3('>');
	if(driving_mode[3]==0|| zmiana_stanu==true){	i=0;
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
	send_char3('#');send_char3('x');send_char3('<');send_char3('!');send_char3('>');send_char3('y');send_char3('<');send_char3('!');send_char3('>');
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
	send_char3('#');send_char3('x');send_char3('<');send_char3('~');send_char3('>');send_char3('y');send_char3('<');send_char3('~');send_char3('>');
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
	send_char3('#');send_char3('x');send_char3('<');send_char3('P');send_char3('>');send_char3('y');send_char3('<');send_char3('P');send_char3('>');
	if(driving_mode[6]==0){	i=0;	//gdy zatrzymujemy sie to trzeba sprawdzic w ktora strone krecil sie wal, do skutecznego zatrzymania
		if(driving_mode[0]==1 || driving_mode[2]==1 || driving_mode[5]==1)		driving_mode[6]=1;
		else if(driving_mode[1]==1 || driving_mode[3]==1 || driving_mode[4]==1) driving_mode[6]=2;

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
