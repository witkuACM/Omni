/*
 * steerage.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#include "steerage.h"
#include <stdlib.h>
#include "variables.h"
#include "driving_functions.h"
#include "delay.h"
#include "uart_interrupt.h"

void sterowanie_xy(volatile uint8_t buffer[]){
	if(FLAG_CORRECTION_OF_INSTR==true && FLAG_END_OF_INSTR==true){

		Vx=buffer[3];																				//PREDKOSC PRAWO - LEWO
		Vy=buffer[7];																				//PREDKOSC PRZOD - TYL

		if(Vx_last != Vx || Vy_last != Vy) zmiana_stanu = true;
		else	zmiana_stanu=false;

		Vx_last=Vx;
		Vy_last=Vy;


		if(Vx<0x21 || Vx>0x7E)		stop();															//wartosci wykraczajace poza zakres Vx
		else if(Vy<0x21 || Vy>0x7E)	stop();															//wartosci wykraczajace poza zakres Vy
		else{
			wypelnienie_x = (abs(80-Vx)*500)/46;													//ALGORYTM OBLICZANIA WYPELNIENIA
			wypelnienie_y = (abs(80-Vy)*500)/46;													//TERAZ ZAKRES <33;126> a SRODEK w 80(DECYMALNY)

			if(Vx==0x50 && Vy==0x50){					stop();}									//spoczynek
			else if(Vx>0x50  && Vy>=0x47 && Vy<=0x53){	prawo(wypelnienie_x);}						//PRAWO z Vx - wypelnienie_x
			else if(Vx<0x50  && Vy>=0x47 && Vy<=0x53){	lewo(wypelnienie_x);}						//LEWO z Vx - wypelnienie_x
			else if(Vy>0x50  && Vx==0x50){				przod(wypelnienie_y*3/4);}					//PRZOD z Vy - wypelnienie_y
			else if(Vy<0x50  && Vx==0x50){				tyl(wypelnienie_y*3/4);}					//TYL z Vy - wypelnienie_y
			else if(Vy==0x21 && Vx==0x21){				zwrotLewo();}								//OBROT w LEWO - wypelnienie 1000
			else if(Vy==0x7E && Vx==0x7E){				zwrotPrawo();}								//OBROT W PRAWO - wypelnienie 1000
			else if(Vx>0x50  && Vy>0x53) {				przod(wypelnienie_x/4 + wypelnienie_y*3/4);}//PRZOD + PRAWO
			else if(Vx>0x50  && Vy<0x47) {				tyl(wypelnienie_x/4 + wypelnienie_y*3/4);}	//TYL + PRAWO
			else if(Vx<0x50  && Vy<0x47) {				tyl(wypelnienie_y*3/4);}					//TYL + LEWO
			else if(Vx<0x50  && Vy>0x53) {				przod(wypelnienie_y*3/4);} 					//PRZOD + LEWO
		}
	}
	else{
		nic();	//NIC albo STOP w zaleznosci czy ruch czy spoczynek przy wpisywaniu kolejnej komendy
	}
	//delay_ms(50);		//trzeba cos z tym zrobic, NIEPOTRZEBNE
}
