/*
 * zmienne.h
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

#include <stdio.h>
#include <stdbool.h>

//wielkosc ramki - UART
#define 	START_SIZE			3
#define		STOP_SIZE			3
#define 	DATA_SIZE			1
#define		BUFFER_SIZE			START_SIZE+STOP_SIZE+DATA_SIZE

#define 	znak_pocz 	 		0x5B			// '['
#define 	znak_kropka  		0x2E			// '.'
#define	 	znak_kon 			0x5D			// ']'
#define 	znak_null 			0x00			// NULL

bool 	FLAG_START;					//flaga START
bool 	FLAG_STOP;					//flaga STOP
bool 	FLAG_DATA;					//flaga DATA

volatile uint8_t tab[BUFFER_SIZE];	//tablica - cala ramka po UART
volatile uint8_t tab_dma[DATA_SIZE];//tablica jednoznakowa - komenda z UART przesylana przez DMA

volatile uint16_t wypelnienie;		//zmienna - wartosc PWM podawana na silnik

#endif /* VARIABLES_H_ */
