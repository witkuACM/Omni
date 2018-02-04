/*
 * steerage.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#include "steerage.h"
#include "variables.h"
#include "driving_functions.h"
#include "delay.h"
#include "uart_interrupt.h"

void sterowanie(volatile uint8_t buffer[]){
		if(FLAG_START==true && FLAG_DATA==true && FLAG_STOP==true){

			// ROZNE POLECENIA 1-znakowe
			 switch (buffer[0])
			 {
				 case 'w':
					przod();
					break;
				 case 's':
					tyl();
					break;
				 case 'a':
					lewo();
					break;
				 case 'd':
					prawo();
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
}
