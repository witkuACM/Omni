/*
 * pid.h
 *
 *  Created on: 28 lis 2017
 *      Author: wiciu
 */

#ifndef PID_H_
#define PID_H_

extern double pid_calculate(double set_val, double read_val);
extern double zamiana_jednostek(double value_obr_min);

#endif /* PID_H_ */
