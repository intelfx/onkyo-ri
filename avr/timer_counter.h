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
 * This file declares hardware timer 0 operations of the RI decoder firmware.
 */

#pragma once

#include <stdint.h>
#include <avr/io.h>

#define TIMER_COUNTER_PRESCALER    256
#define TIMER_COUNTER_CS_PRESCALER (1 << CS02) | (0 << CS01) | (0 << CS00)
#define TIMER_COUNTER_CS_MASK      (1 << CS02) | (1 << CS01) | (1 << CS00)
#define TIMER_COUNTER_FREQ         (F_CPU / TIMER_COUNTER_PRESCALER)

extern volatile uint16_t timer_counter;

void timer_counter_init(void);
void timer_counter_set_period(uint8_t ticks);
void timer_counter_start(void);
void timer_counter_stop(void);
