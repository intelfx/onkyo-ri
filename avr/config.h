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
 * This file contains "config" statements of the RI decoder firmware
 * binding it to the hardware properties.
 */

#pragma once

// LEDx specify the physical I/O port and pin connected to the x-th status LED.
// Nice hack: two parameters via a comma, so that one can call io_make_*(&LED0) and it will do the right thing
#define LED PORTB, 5
#define RI PORTD, 2

// TODO: when comparing counter ticks with milliseconds, calculate and use
//       "real" COUNTER_TICK_MS to account for rounding errors in MS_TO_CLOCKS().
#define COUNTER_TICK_MS 0.25
