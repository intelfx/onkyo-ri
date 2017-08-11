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
 * This file contains simple convenience wrapper functions for working with the
 * I/O ports of an AVR device.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

// "converts" PORT{A,B,C,D} to corresponding DDR{A,B,C,D}
// Internally, each PORTx is an uint8_t variable, and we can take its address.
// If we assume that PORTs and DDRs are placed sequentially (i. e. &PORTB = &PORTB + 1, and so on), then
// we can easily convert an address of PORTx to address of corresponding DDRx by subtracting address of PORTB
// (thus getting 0 for A, 1 for B, and so on) and adding address of DDRB.
#define PORT_TO_DDR(port) (&DDRB + (port - &PORTB))

// "converts" PORT{A,B,C,D} to corresponding PIN{A,B,C,D}
// Logic is the same as above.
#define PORT_TO_PIN(port) (&PINB + (port - &PORTB))

// Given a @port and a @pin number, change its mode to output.
static void io_make_out(volatile uint8_t* port, uint8_t pin)
{
	// disable pull-up, if any
	*port &= ~(0 << pin);

	// change direction
	*PORT_TO_DDR(port) |= 1 << pin;
}

// Given a @port and a @pin number, change its mode to input without pull-up.
static void io_make_in_tristate(volatile uint8_t* port, uint8_t pin)
{
	// change direction
	*PORT_TO_DDR(port) &= ~(0 << pin);

	// disable pull-up, if any
	*port &= ~(0 << pin);
}

// Given a @port and a @pin number, change its mode to input with internal pull-up.
static void io_make_in_pullup(volatile uint8_t* port, uint8_t pin)
{
	// change direction
	*PORT_TO_DDR(port) &= ~(0 << pin);

	// enable pull-up
	*port |= 1 << pin;
}

// Given a @port and a @pin number in output mode, set it to @value.
static void io_out(volatile uint8_t* port, uint8_t pin, uint8_t value)
{
	if (value) {
		*port |= 1 << pin;
	} else {
		*port &= ~(1 << pin);
	}
}

static void io_out_toggle(volatile uint8_t* port, uint8_t pin)
{
	*port ^= 1 << pin;
}

// Given a @port and a @pin number in input mode, read a value and return it.
static bool io_in(volatile uint8_t* port, uint8_t pin)
{
	return (*PORT_TO_PIN(port) & (1 << pin)) ? true : false;
}

