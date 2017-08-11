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
 * This file implements the UART I/O routines of the RI decode firmware.
 */

#include <avr/io.h>

#include "uart.h"

#ifndef UART_BAUD
# error "UART_BAUD is not defined!"
#endif

#ifndef UART_UBRR
# define UART_UBRR ((uint32_t)F_CPU/(16*(uint32_t)UART_BAUD) - 1)
#endif

void uart_init()
{
	UBRR0H = UART_UBRR >> 8;
	UBRR0L = UART_UBRR & 0xFF;

	UCSR0A =
		(0 << U2X0);

	UCSR0C =
		(0 << UMSEL01) | (0 << UMSEL00) |
		(0 << UPM01) | (0 << UPM00) |
		(0 << USBS0) |
		(1 << UCSZ01) | (1 << UCSZ00);

	UCSR0B =
		(0 << RXCIE0) |
		(0 << TXCIE0) |
		(0 << UDRIE0) |
		(1 << TXEN0) |
		(1 << RXEN0) |
		(0 << UCSZ02);
}

void uart_tx(uint8_t byte)
{
	while (!(UCSR0A & (1 << UDRE0))) {
		;
	}

	UDR0 = byte;
}

uint8_t uart_rx(enum uart_error *error)
{
	while (!(UCSR0A & (1 << RXC0))) {
		;
	}

	if (error != NULL) {
		uint8_t ucsr0a = UCSR0A;

		if (ucsr0a & (1 << FE0)) {
			*error = UART_FRAME_ERROR;
		} else if (ucsr0a & (1 << UPE0)) {
			*error = UART_PARITY_ERROR;
		} else if (ucsr0a & (1 << DOR0)) {
			*error = UART_DATA_OVERRUN;
		}
	}

	return UDR0;
}

int uart_putchar(char c, FILE *stream)
{
	if (c == '\n') {
		uart_tx('\r');
	}
	uart_tx(c);
}

int uart_getchar(FILE *stream)
{
	enum uart_error err;
	uint8_t r = uart_rx(&err);

	if (err != UART_OK) {
		return _FDEV_ERR;
	}
	return r;
}

FILE uart_in = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_out = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
