/*
 * This file is part of onkyo-ri.
 *
 * Copyright 2016 Ivan Shapovalov <intelfx@intelfx.name>
 *
 * onkyo-ri is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 3
 * as published by the Free Software Foundation.
 *
 * onkyo-ri is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with onkyo-ri.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This file implements the time counter of the RI decoder firmware.
 * We use the hardware timer 0 in the CTC mode to provide a plain "stopwatch"
 * with a configurable period.
 */

#include <avr/interrupt.h>

#include "timer_counter.h"
#include "util.h"

volatile uint16_t timer_counter;

void timer_counter_init(void)
{
	TCCR0A =
		// 0b010 -- Clear Timer on Compare mode
		/* (x << WGM02) | */ (1 << WGM01) | (0 << WGM00);

	TCCR0B =
		// 0b010 -- Clear Timer on Compare mode
		(0 << WGM02);
		// no Clock Select
}

void timer_counter_set_period(uint8_t ticks)
{
#define TIMER_COUNTER_TICK_MS ((double)1000 / TIMER_COUNTER_FREQ)
	log("timer_counter_set_period: freq %u, %u ticks = %u.%03u ms",
	    (unsigned)TIMER_COUNTER_FREQ,
	    (unsigned)ticks,
	    (unsigned)ticks * (unsigned)(TIMER_COUNTER_TICK_MS * 1000) / 1000,
	    (unsigned)ticks * (unsigned)(TIMER_COUNTER_TICK_MS * 1000) % 1000);

	OCR0A = ticks - 1;
}

void timer_counter_start(void)
{
	/*
	 * 1. reset prescaler (gives us 16/64/256/1024 CPU clocks before the first prescaler tick);
	 * 2. clear the hardware counter (TCNT0);
	 * 3. enable the timer and start counting (gives us OCR0A clocks before first interrupt);
	 * 4. clear the software counter (event count).
	 */

	GTCCR |=
		// Prescaler Reset for Timer/Counter 0
		(1 << PSRSYNC);

	TCNT0 = 0;

	TCCR0B |=
		// Clock Select
		TIMER_COUNTER_CS_PRESCALER;

	timer_counter = 0;

	TIMSK0 |=
		// Timer/Counter 0 Output Compare Match A Interrupt Enable
		(1 << OCIE0A);
}

void timer_counter_stop(void)
{
	TCCR0B &=
		// Clock Select
		~TIMER_COUNTER_CS_MASK;

	TIMSK0 &=
		// Timer/Counter 0 Output Compare Match A Interrupt Enable
		~(1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect)
{
	++timer_counter;
}
