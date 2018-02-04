/*
 * driving_functions.h
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#ifndef DRIVING_FUNCTIONS_H_
#define DRIVING_FUNCTIONS_H_

extern void przod(uint16_t v);
extern void tyl(uint16_t v);
extern void lewo(uint16_t v);
extern void prawo(uint16_t v);
extern void zwrotLewo();
extern void zwrotPrawo();
extern void stop();
extern void nic();

#endif /* DRIVING_FUNCTIONS_H_ */
