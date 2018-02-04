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

//parametry regulatora PID
#define 	kp 		15//17.9405 			//20
#define 	ki		1.5//268.2343*0.005		//1		teraz 1.34
#define 	kd		0//0.029668			//0.5
#define 	ERR_SUM_MAX		1000

volatile double err;					//uchyb
volatile double err_sum;				//suma wszystkich uchybow
volatile double err_last;				//ostatnia wartosc uchybu
volatile double err_d;					//roznica miedzy ostatnim a obecnym uchybem
volatile double u;						// u = kp*err + ki*err_sum + kd*err_d
volatile double e;
volatile int i;							//zmienna do petli for
volatile int driving_mode[7];			//zmienna do driving functions

volatile uint16_t pid_value;			//wypelnienie obliczone z PID
volatile double set_value;				//zadana ilosc tykniec enkodera na 1ms
volatile double enc_value;				//zczytana --||--
volatile double set_value_obrot_min;	//zadana predkosc katowa wyrazona w obr/min

volatile double enc_value_diff[400];	//liczba tykniec enkodera przy starcie co 1ms

volatile uint16_t pulse_count1;			//zczytane dwa stany enkodera, odstep 1ms
volatile uint16_t pulse_count2;

//wielkosc ramki - UART
#define 	START_SIZE			3
#define		STOP_SIZE			3
#define 	DATA_SIZE			1
#define		BUFFER_SIZE			START_SIZE+STOP_SIZE+DATA_SIZE

#define 	znak_pocz 	 		0x5B			// '['
#define 	znak_kropka  		0x2E			// '.'
#define	 	znak_kon 			0x5D			// ']'
#define 	znak_null 			0x00			// NULL

volatile bool 	FLAG_START;					//flaga START
volatile bool 	FLAG_STOP;					//flaga STOP
volatile bool 	FLAG_DATA;					//flaga DATA

volatile uint8_t tab[BUFFER_SIZE];	//tablica - cala ramka po UART
volatile uint8_t tab_dma[DATA_SIZE];//tablica jednoznakowa - komenda z UART przesylana przez DMA

#endif /* VARIABLES_H_ */
