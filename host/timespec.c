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
 * This file implements a few helper routines working with struct timespec.
 */

#include "timespec.h"

struct timespec timespec_subtract(const struct timespec *_1, const struct timespec *_2)
{
	struct timespec r = *_1;

	r.tv_sec -= _2->tv_sec;
	if (r.tv_nsec < _2->tv_nsec) {
		assert(r.tv_sec > 0);
		r.tv_nsec += 1e9;
		--r.tv_sec;
	}
	r.tv_nsec -= _2->tv_nsec;

	return r;
}

struct timespec timespec_add(const struct timespec *_1, const struct timespec *_2)
{
	struct timespec r = *_1;

	r.tv_sec += _2->tv_sec;
	r.tv_nsec += _2->tv_nsec;
	if (r.tv_nsec > 1e9) {
		r.tv_nsec -= 1e9;
		++r.tv_sec;
	}

	return r;
}

bool timespec_ge(const struct timespec *_1, const struct timespec *_2)
{
	if (_1->tv_sec > _2->tv_sec) {
		return true;
	}
	if (_1->tv_sec == _2->tv_sec && _1->tv_nsec >= _2->tv_nsec) {
		return true;
	}
	return false;
}
