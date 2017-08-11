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
 * This file contains declarations for the event queue of the RI decoder firmware.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

enum event_type
{
	NO_EVENT = 0,
	PIN_CHANGE,
};

struct event
{
	uint16_t timestamp;
	enum event_type event;
	union {
		struct {
			bool value;
		} pin_change;
	};
};

extern struct event event_queue[];

/*
 * Returns a copy of the first event in queue, removing it from the queue.
 */
struct event event_get(void);

/*
 * Appends an empty event to the queue, returning a pointer to the new event,
 * which must be filled by the caller.
 * The caller must ensure it does not event_get() the new event until it has
 * been filled.
 */
struct event *event_put(void);
