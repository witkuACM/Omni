/*
 * pid.c
 *
 *  Created on: 28 lis 2017
 *      Author: wiciu
 */

#include "variables.h"
#include "pid.h"

double pid_calculate(double set_val, double read_val)
{
	err = set_val - read_val;
	err_sum = err_sum +	err;

	if (err_sum > ERR_SUM_MAX) {
			err_sum = ERR_SUM_MAX;
	} else if (err_sum < -ERR_SUM_MAX) {
			err_sum = -ERR_SUM_MAX;
	}

	err_d = err_last - err;

	u = kp * err + ki * err_sum + kd * err_d;

	err_last = err;

	if (u > ERR_SUM_MAX) {				//ograniczenie sygna³u wyjœciowego reg.
			u = ERR_SUM_MAX;
	} else if (u < 0) {
			u = 0;
	}

	return u;
}

double zamiana_jednostek(double value_obr_min){
	set_value_obrot_min = value_obr_min;

	if(set_value_obrot_min>500) set_value=10.0;		//odpowiednik 10 tykniec enkodera na 1ms
	else if(set_value_obrot_min<25 && set_value_obrot_min!=0)	set_value=0.5;
	else	set_value = set_value_obrot_min/50.0;

	return set_value;
}
