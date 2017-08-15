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
 * This file declares a few helper routines working with struct timespec.
 */

#pragma once

#define _GNU_SOURCE
#include <stdbool.h>
#include <assert.h>

#include <time.h>

struct timespec timespec_subtract(const struct timespec *_1, const struct timespec *_2);
struct timespec timespec_add(const struct timespec *_1, const struct timespec *_2);
bool timespec_ge(const struct timespec *_1, const struct timespec *_2);
