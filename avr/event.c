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
 * This file contains implementation of the event queue of the RI decoder firmware.
 */

#include <stdint.h>
#include <util/atomic.h>

#include "event.h"
#include "util.h"

#define EVENT_QUEUE_LENGTH 100

struct event event_queue[EVENT_QUEUE_LENGTH];
uint8_t event_queue_head = 0; // points to the next free place
uint8_t event_queue_tail = 0; // points to the last usable element
uint8_t event_queue_count = 0;

void event_check()
{
	assert(event_queue_head < elementsof(event_queue));
	assert(event_queue_tail < elementsof(event_queue));
	assert(event_queue_count <= elementsof(event_queue));
}

/*
 * Possible optimization: return a pointer to the just-freed entry without
 * re-enabling interrupts, so that the caller may either quickly process the
 * result, avoiding copying, or copy it manually and re-enable interrupts.
 */
struct event event_get()
{
	struct event r;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		event_check();

		if (event_queue_count != 0) {
			r = event_queue[event_queue_tail];

			++event_queue_tail;
			if (event_queue_tail >= elementsof(event_queue)) {
				event_queue_tail = 0;
			}

			--event_queue_count;
		} else {
			r.event = NO_EVENT;
		}
	}
	
	return r;
}

/*
 * Optimization: event_put() only allocates an entry, which is later filled
 * by the caller. Since event_get() is never called from the ISRs, we will
 * never event_get() a half-filled event entry.
 */
struct event *event_put()
{
	struct event *r;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		event_check();
		assert(event_queue_head < elementsof(event_queue));

		r = &event_queue[event_queue_head];

		++event_queue_head;
		if (event_queue_head >= elementsof(event_queue)) {
			event_queue_head = 0;
		}

		++event_queue_count;
	}

	return r;

}
