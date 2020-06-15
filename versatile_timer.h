/*
 * timer.h
 *
 *  Created on: 24 mars 2020
 *      Author: mi11
 */

#ifndef VERSATILE_TIMER_H_
#define VERSATILE_TIMER_H_

void timer_priodic_setup(int ntimer, int period_ms);
void timer_int_clear(int ntimer);
void timer_wait(int ntimer);

#endif /* VERSATILE_TIMER_H_ */
