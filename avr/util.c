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
 * This file contains common utility functions of the RI decoder firmware.
 */

#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "config.h"
#include "util.h"
#include "io.h"

void __attribute__((noreturn)) abort_msg(const char *message)
{
	cli();
	io_make_out(&LED);

#ifdef DEBUG
	if (message != NULL) {
		fputs(message, stderr);
	}
#endif

	for (;;) {
		io_out(&LED, 1);
		_delay_ms(500);
		io_out(&LED, 0);
		_delay_ms(500);
	}
}
