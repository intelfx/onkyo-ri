/*
 * This file is part of onkyo-ri.
 *
 * Copyright 2017 Ivan Shapovalov <intelfx@intelfx.name>
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
 * This file declares the UART I/O routines of the RI decode firmware.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum uart_error
{
	UART_OK = 0,
	UART_FRAME_ERROR,
	UART_DATA_OVERRUN,
	UART_PARITY_ERROR,
};

void uart_init(void);
void uart_tx(uint8_t byte);
uint8_t uart_rx(enum uart_error *error);

extern FILE uart_in, uart_out;
