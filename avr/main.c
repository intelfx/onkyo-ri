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
 * This file implements the high-level logic of the RI decoder firmware,
 * intended to run on an AVR device.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "config.h"
#include "io.h"
#include "timer_counter.h"
#include "util.h"
#include "uart.h"
#include "event.h"

void setup(void)
{
	/*
	 * General I/O configuration
	 */

	io_make_out(&LED);
	io_make_in_tristate(&RI);

	/*
	 * Power Reduction (PRR)
	 */

	PRR =
		// 0b1 = No Timer/Counter 1
		(1 << PRTIM1) |
		// 0b1 = No Timer/Counter 2
		(1 << PRTIM2) |
		// 0b1 = No TWI
		(1 << PRTWI) |
		// 0b1 = No SPI
		(1 << PRSPI) |
		// 0b1 = No ADC
		(1 << PRADC);

	/*
	 * Sleep Mode
	 */

	set_sleep_mode(0b000);
	sleep_enable();

	/*
	 * Hardware timer as a time counter
	 */

	timer_counter_init();

	/*
	 * Main input -- external interrupt 0
	 */

	EICRA =
		// 0b01 = Any logical change on INT0
		(0 << ISC01) | (1 << ISC00);

	EIMSK =
		// 0b1 = External Interrupt Request 0 Enable
		(1 << INT0);

	/*
	 * UART
	 */

	uart_init();
	stdin = &uart_in;
	stdout = &uart_out;
	stderr = &uart_out;

	sei();
}

ISR(INT0_vect)
{
	struct event *e = event_put();
	e->timestamp = timer_counter;
	e->event = PIN_CHANGE;
	e->pin_change.value = io_in(&RI);
}

enum ri_fsm_state
{
	STATE_INITIAL = 0,
	STATE_HEADER_HIGH,
	STATE_HEADER_LOW,
	STATE_BIT_HIGH,
	STATE_BIT_LOW,
	STATE_TRAILER_HIGH,
};

enum ri_fsm_state ri_fsm_state;
uint16_t ri_fsm_pin_change_timestamp = 0;
uint16_t ri_fsm_tmp_value = 0;
uint8_t ri_fsm_tmp_bit_count = 0;

void ri_received(uint16_t value)
{
	log("RI value received: 0x%04x", value);
	printf("INPUT: 0x%04x\n", value);
}

void ri_fsm(bool value, uint16_t pin_change_delta)
{
#define DELTA_GREATER(low) \
	(pin_change_delta >= MS_TO_CLOCKS(low, COUNTER_TICK_MS*1000))

#define DELTA_LESS(high) \
	(pin_change_delta <= MS_TO_CLOCKS(high, COUNTER_TICK_MS*1000))

#define DELTA_IN_RANGE(low, high) \
	(DELTA_GREATER(low) && DELTA_LESS(high))

// no `do { ... } while (0)` idiom here because we want to break from the outer switch
#define RI_FSM_GO(new_state) \
	{ \
		log("FSM: -> " #new_state); \
		ri_fsm_state = new_state; \
		break; \
	}

// no `do { ... } while (0)` idiom here because we want to break from the outer switch
#define RI_FSM_ERROR() \
	{ \
		log("FSM: ERROR"); \
		ri_fsm_state = STATE_INITIAL; \
		break; \
	}

	switch (ri_fsm_state) {
	case STATE_INITIAL:
		if (value && DELTA_GREATER(20)) {
			RI_FSM_GO(STATE_HEADER_HIGH);
		} else {
			RI_FSM_ERROR();
		}
		break;

	case STATE_HEADER_HIGH:
		if (!value && DELTA_IN_RANGE(2.5, 3.5)) {
			RI_FSM_GO(STATE_HEADER_LOW);
		} else {
			RI_FSM_ERROR();
		}

	case STATE_HEADER_LOW:
		if (value && DELTA_IN_RANGE(0.5, 1.5)) {
			RI_FSM_GO(STATE_BIT_HIGH);
		} else {
			RI_FSM_ERROR();
		}

	case STATE_BIT_HIGH:
		if (!value && DELTA_IN_RANGE(0.5, 1.5)) {
			RI_FSM_GO(STATE_BIT_LOW);
		} else {
			RI_FSM_ERROR();
		}

	case STATE_BIT_LOW:
		if (value && DELTA_IN_RANGE(0.5, 1.5)) {
			// bit received: zero
			ri_fsm_tmp_value <<= 1;
			++ri_fsm_tmp_bit_count;
		} else if (value && DELTA_IN_RANGE(1.5, 2.5)) {
			// bit received: one
			ri_fsm_tmp_value <<= 1;
			ri_fsm_tmp_value |= 1;
			++ri_fsm_tmp_bit_count;
		} else {
			RI_FSM_ERROR();
		}

		// if we are here, it means that we have received a bit
		if (ri_fsm_tmp_bit_count < 12) {
			// receive next bit
			RI_FSM_GO(STATE_BIT_HIGH);
		} else {
			// we're done
			RI_FSM_GO(STATE_TRAILER_HIGH);
		}

	case STATE_TRAILER_HIGH:
		if (!value && DELTA_IN_RANGE(0.5, 1.5)) {
			ri_received(ri_fsm_tmp_value);
			ri_fsm_tmp_value = 0;
			ri_fsm_tmp_bit_count = 0;
			RI_FSM_GO(STATE_INITIAL);
		} else {
			RI_FSM_ERROR();
		}

	default:
		RI_FSM_ERROR();
	}
}

int main(void)
{
	setup();
	log("setup() completed");

	timer_counter_set_period(MS_TO_CLOCKS(COUNTER_TICK_MS, TIMER_COUNTER_FREQ));
	timer_counter_start();

	uint16_t pin_change_timestamp = 0;
	uint16_t pin_change_delta = 0;

	for (;;) {
		sleep_cpu();
		if (!(timer_counter % 1000)) {
			log("got interrupt, timer_counter = %u", timer_counter);
		}

		struct event e = event_get();
		switch (e.event) {
		case NO_EVENT:
			continue;

		case PIN_CHANGE:
			/*
			 * Note that this correctly handles (single) overflows:
			 * if timer counter overflowed and e.timestamp < pin_change_timestamp,
			 * the subtraction will over(under)flow too, yielding a
			 * correct result (i. e. 1 - 0xFFFF = 2).
			 * If the counter overflowed more than once, things won't work
			 * (that's why we pick a 16-bit counter).
			 */
			pin_change_delta = e.timestamp - pin_change_timestamp;

			log("pin change: %u, %u ticks = %u.%02u ms since previous",
			    (unsigned)e.pin_change.value,
			    (unsigned)pin_change_delta,
			    (unsigned)pin_change_delta * (unsigned)(COUNTER_TICK_MS * 100) / 100,
			    (unsigned)pin_change_delta * (unsigned)(COUNTER_TICK_MS * 100) % 100);

			ri_fsm(e.pin_change.value, pin_change_delta);
			pin_change_timestamp = e.timestamp;
			break;
		}
	}
}
