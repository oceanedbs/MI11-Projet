/*
 * timer.c
 *
 *  Created on: 24 mars 2020
 *      Author: mi11
 */

#include <stdint.h>

#define TMRCLK (1000000)

#define TMRCTRL_TIMEREN (1 << 7)
#define TMRCTRL_MODE_FREE (0 << 6)
#define TMRCTRL_MODE_PERIODIC (1 << 6)
#define TMRCTRL_INTEN (1 << 5)
#define TMRCTRL_PRE1 (0 << 2)
#define TMRCTRL_PRE4 (1 << 2)
#define TMRCTRL_PRE256 (2 << 2)
#define TMRCTRL_TIMERSIZE32 (1 << 1)
#define TMRRIS (1)

typedef volatile struct sp804_timer {
	uint32_t tmr_load;
	uint32_t tmr_val;
	uint32_t tmr_ctrl;
	uint32_t tmr_int_crl;
	uint32_t tmr_ris;
	uint32_t tmr_mis;
} sp804_timer_t;

sp804_timer_t* sp804_timers[] = {
		(void *) 0x101e2000,
		(void *) 0x101e2020,
		(void *) 0x101e3000,
		(void *) 0x101e3020,
};

void timer_priodic_setup(int ntimer, int period_ms)
{
	sp804_timer_t* timer = sp804_timers[ntimer];

	timer->tmr_ctrl &= ~TMRCTRL_TIMEREN;
	timer->tmr_ctrl = TMRCTRL_PRE1 | TMRCTRL_TIMERSIZE32 | TMRCTRL_MODE_PERIODIC | TMRCTRL_INTEN;
	timer->tmr_load = period_ms * TMRCLK / 1000;
	timer->tmr_ctrl |= TMRCTRL_TIMEREN;
}

void timer_int_clear(int ntimer)
{
	sp804_timer_t* timer = sp804_timers[ntimer];
	timer->tmr_int_crl = 0;

}

void timer_wait(int ntimer)
{
	sp804_timer_t* timer = sp804_timers[ntimer];

	while (!(timer->tmr_ris & TMRRIS)) continue;
	timer_int_clear(ntimer);
}

