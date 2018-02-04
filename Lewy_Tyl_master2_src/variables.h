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
#define 	kp 		15
#define 	ki		15*0.1
#define 	kd		0
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

// CZUJNIKI
#define 	ADC_CHANNELS 	6 							//liczba czujnikow analogowych, z ktorych odczytujemy dane
#define 	vmax_przod		1300
#define 	vmax_reszta		1400	//1400 nie za bardzo bo gdy przeszkoda bardzo blisko czujnika to wartosci maleja do 1200 okolo
#define 	lbnad			0
#define		lfnad			1
#define		lf				2
#define		rf				3
#define		rfnad			4
#define		rbnad			5

volatile uint16_t adc_value[ADC_CHANNELS];				//bufor do DMA na odczyty z czujnikow
volatile uint16_t blok_przod,blok_tyl,blok_prawo,blok_lewo;

volatile bool blokowanie_przod;
volatile bool blokowanie_lewo;
volatile bool blokowanie_prawo;
volatile bool blokowanie_tyl;

#endif /* VARIABLES_H_ */
