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

void czysc_dane(volatile uint8_t buffer[]){
	for(int i=START_SIZE;i<START_SIZE+DATA_SIZE;i++){
		buffer[i]=znak_null;
	}
}

void ramka(volatile uint8_t buffer[]){			//ramka [..x..]
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

void sprawdz_ramke(volatile uint8_t buffer[]){
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

void start_instrukcji(){

	if(FLAG_START==true && FLAG_DATA==true && FLAG_STOP==true){
		tab_instrukcje[k]=tab[START_SIZE];
	}
}

void sprawdz_ramke_xy(volatile uint8_t buffer[]){
	if(buffer[0]=='#' && buffer[1]=='x' && buffer[2]=='<' && buffer[3]!=znak_null && buffer[4]=='>' && buffer[5]=='y' && buffer[6]=='<' && buffer[7]!=znak_null && buffer[8]=='>'){
		FLAG_CORRECTION_OF_INSTR=true;
	}
	else{FLAG_CORRECTION_OF_INSTR=false;}
}
