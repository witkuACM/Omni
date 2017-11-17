/*
 * frame.c
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#include <stdbool.h>
#include "frame.h"
#include "variables.h"

void czysc_flagi(){
	FLAG_START	=	false;			//flaga START
 	FLAG_STOP	=	false;			//flaga STOP
	FLAG_DATA	=	false;			//flaga DATA
}

void czysc_dane(uint8_t buffer[]){
	for(int i=START_SIZE;i<START_SIZE+DATA_SIZE;i++){
		buffer[i]=znak_null;
	}
}

void ramka(uint8_t buffer[]){			//ramka [..x..]
	buffer[0]=znak_pocz;
	for(int i=1;i<START_SIZE;i++){
		buffer[i]=znak_kropka;
	}

	//miejsce na znak z USART2

	for(int i=START_SIZE+DATA_SIZE;i<BUFFER_SIZE-1;i++){
		buffer[i]=znak_kropka;
	}
	buffer[BUFFER_SIZE-1]=znak_kon;
}

void sprawdz_ramke(uint8_t buffer[]){
	if(buffer[0]=='[' && buffer[1]=='.' && buffer[2]=='.'){
		FLAG_START = true;

		if(buffer[START_SIZE]!=znak_null){
			FLAG_DATA = true;

			if(buffer[START_SIZE+DATA_SIZE]=='.' && buffer[START_SIZE+DATA_SIZE+1]=='.' && buffer[BUFFER_SIZE-1]==']'){
				FLAG_STOP = true;
			}
			else{czysc_flagi();}
		}
		else{czysc_flagi();}
	}
	else{czysc_flagi();}
}
