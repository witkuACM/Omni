/*
 * config.h
 *
 *  Created on: 12 lis 2017
 *      Author: wiciu
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

extern void RCCinit();
extern void GPIOinit();
extern void UARTinit();
extern void TIMinit();
extern void DMAinit();
extern void NVICinit();
extern void PeriphClock();
extern void ADCinit();
extern void EXTIinit();
extern void Multipleks();

#endif /* CONFIGURATION_H_ */
