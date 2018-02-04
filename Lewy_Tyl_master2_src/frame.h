/*
 * frame.h
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <stdio.h>

extern void czysc_flagi();
extern void czysc_dane(volatile uint8_t buffer[]);
extern void ramka(volatile uint8_t buffer[]);
extern void sprawdz_ramke(volatile uint8_t buffer[]);

#endif /* FRAME_H_ */
