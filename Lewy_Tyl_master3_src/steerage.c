/*
 * steerage.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */
#include "stm32f10x.h"
#include <stdlib.h>
#include "steerage.h"
#include "variables.h"
#include "driving_functions.h"
#include "systick_interrupt.h"
#include "uart_interrupt.h"

void sterowanie_xy(volatile uint8_t buffer[]){
	if(FLAG_CORRECTION_OF_INSTR==true && FLAG_END_OF_INSTR==true){

		Vx=buffer[3];					//PREDKOSC PRAWO - LEWO
		Vy=buffer[7];					//PREDKOSC PRZOD - TYL

		if(blokowanie_przod == false && blokowanie_tyl == false && blokowanie_lewo == false && blokowanie_prawo == false){

			if(Vx_last != Vx || Vy_last != Vy) zmiana_stanu = true;
			else	zmiana_stanu=false;

			Vx_last=Vx;
			Vy_last=Vy;


			if(Vx<0x21 || Vx>0x7E)		stop();				//wartosci wykraczajace poza zakres Vx
			else if(Vy<0x21 || Vy>0x7E)	stop();				//wartosci wykraczajace poza zakres Vy
			else{
				wypelnienie_x = (abs(80-Vx)*500)/46;		//ALGORYTM OBLICZANIA PREDKOSCI ZADANEJ
				wypelnienie_y = (abs(80-Vy)*500)/46;		//TERAZ ZAKRES <33;126> a SRODEK w 80(DECYMALNY)

				if(Vx==0x50 && Vy==0x50)	stop();								//spoczynek
				else if(Vx>0x50  && Vy>=0x47 && Vy<=0x53){
					if(blok_prawo==1)	blokowanie_prawo = true, send_char1('F'), send_char1('4');	// FLAGA + wysyla komende 	do komputera/raspby
						else				prawo(wypelnienie_x);									//PRAWO z wypelnienie_x
				}
				else if(Vx<0x50  && Vy>=0x47 && Vy<=0x53){
					if(blok_lewo==1)	blokowanie_lewo = true, send_char1('F'), send_char1('3');	// FLAGA + wysyla komende 	do komputera/raspby
						else				lewo(wypelnienie_x);									//LEWO z wypelnienie_x
				}
				else if(Vy>0x50  && Vx==0x50){
					if(blok_przod==1)	blokowanie_przod = true, send_char1('F'), send_char1('1');	// FLAGA + wysyla komende 	do komputera/raspby
						else				przod(wypelnienie_y*3/4);								//PRZOD z 3/4 wypelnienie_y
				}
				else if(Vy<0x50  && Vx==0x50){
					if(blok_tyl==1)		blokowanie_tyl = true, send_char1('F'), send_char1('2');	// FLAGA + wysyla komende 	do komputera/raspby
						else				tyl(wypelnienie_y*3/4);									//TYL z 3/4 wypelnienie_y
				}
				else if(Vy==0x21 && Vx==0x21){				zwrotLewo();}							//OBROT w LEWO - 500obr/min
				else if(Vy==0x7E && Vx==0x7E){				zwrotPrawo();}							//OBROT W PRAWO - 500obr/min
				else if(Vx>0x50  && Vy>0x53) {
					if(blok_przod==1)		blokowanie_przod = true, send_char1('F'), send_char1('1');	// FLAGA + wysyla komende 	do komputera/raspby
					else if(blok_prawo==1)	blokowanie_prawo = true, send_char1('F'), send_char1('4');
						else					przod(wypelnienie_x/4 + wypelnienie_y*3/4);				//PRZOD + PRAWO
				}
				else if(Vx>0x50  && Vy<0x47) {
					if(blok_tyl==1)			blokowanie_tyl = true, send_char1('F'), send_char1('2');	// FLAGA + wysyla komende 	do komputera/raspby
					else if(blok_prawo==1)	blokowanie_prawo = true, send_char1('F'), send_char1('4');
						else 					tyl(wypelnienie_x/4 + wypelnienie_y*3/4);				//TYL + PRAWO
				}
				else if(Vx<0x50  && Vy<0x47) {
					if(blok_tyl==1)			blokowanie_tyl = true, send_char1('F'), send_char1('2');	// FLAGA + wysyla komende 	do komputera/raspby
					else if(blok_lewo==1)	blokowanie_lewo = true, send_char1('F'), send_char1('3');
						else					tyl(wypelnienie_y*3/4);									//TYL + LEWO
				}
				else if(Vx<0x50  && Vy>0x53) {
					if(blok_przod==1)		blokowanie_przod = true, send_char1('F'), send_char1('1');	// FLAGA + wysyla komende 	do komputera/raspby
					else if(blok_lewo==1)	blokowanie_lewo = true, send_char1('F'), send_char1('3');
						else					przod(wypelnienie_y*3/4); 								//PRZOD + LEWO
				}
			}
		}

		else if(blokowanie_przod == true || blokowanie_tyl == true || blokowanie_lewo == true || blokowanie_prawo == true){ stop();
			//PRZYCISK ZEROWANIA BLOKOWANIA - dwie spacje 0x20
			if(Vx==0x20 && Vy==0x20)	blokowanie_przod = false, blokowanie_tyl = false,blokowanie_lewo = false, blokowanie_prawo = false;
		}
	}

	else	nic();	//utrzymanie predkosci kó³
}
