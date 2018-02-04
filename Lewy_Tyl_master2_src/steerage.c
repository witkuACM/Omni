/*
 * steerage.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */
#include "stm32f10x.h"
#include "steerage.h"
#include "variables.h"
#include "driving_functions.h"
#include "systick_interrupt.h"
#include "uart_interrupt.h"

void sterowanie(uint8_t buffer[]){
	if(FLAG_START==true && FLAG_DATA==true && FLAG_STOP==true){

		if(blokowanie_przod == false && blokowanie_tyl == false && blokowanie_lewo == false && blokowanie_prawo == false){

			// ROZNE POLECENIA 1-znakowe
			 switch (buffer[0])
			 {
				 case 'w':				//ustawienie flagi gdy widziana przeszkoda; wys³anie komunikatu powrotnego
					if(blok_przod==1)	blokowanie_przod = true, send_char1('F'), send_char1('1'),send_char3(' ');
						else			przod();
					break;
				 case 's':
					if(blok_tyl==1)		blokowanie_tyl = true, send_char1('F'), send_char1('2'),send_char3(' ');
						else			tyl();
					break;
				 case 'a':
					if(blok_lewo==1)	blokowanie_lewo = true, send_char1('F'), send_char1('3'),send_char3(' ');
						else			lewo();
					break;
				 case 'd':
					if(blok_prawo==1)	blokowanie_prawo = true, send_char1('F'), send_char1('4'),send_char3(' ');
						else			prawo();
					break;
				 case 'q':
					zwrotLewo();
					break;
				 case 'e':
					zwrotPrawo();
					break;
				 case ' ':
					stop();
					break;
				 default:
					 nic();
					break;
			 }
		}

		//Dzia³anie w razie zablokowania jazdy
		else if(blokowanie_przod == true || blokowanie_tyl == true || blokowanie_lewo == true || blokowanie_prawo == true){ stop();
			switch (buffer[0])
			{
				// KOMENDA 'x' ZWALNIA FLAGÊ BLOKADY
				case 'x':
					blokowanie_przod = false; blokowanie_tyl = false; blokowanie_lewo = false; blokowanie_prawo = false;
				break;
			}
		}
	}
}
