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

#pragma once

#include <stddef.h>
#include <stdio.h>

#define elementsof(a) (sizeof((a)) / sizeof((a)[0]))

#define MS_TO_CLOCKS(ms, freq) ((double)(freq) * (double)(ms) / 1000)

#ifdef DEBUG
# define log(fmt, args...) fprintf(stderr, "LOG: " fmt "\n", ## args)
# define assert(expr) do { if (!(expr)) { abort_msg("ABORT: (" #expr ") == 0\n"); } } while (0)
# define assert_msg(expr, msg) do { if (!(expr)) { abort_msg("ABORT: " msg "\n"); } } while (0)
#else
# define log(fmt, args...) do {} while (0)
# define assert(expr) do { if (!(expr)) { abort_msg(NULL); } } while (0)
# define assert_msg(expr, msg) do { if (!(expr)) { abort_msg(NULL); } } while (0)
#endif


void __attribute__((noreturn)) abort_msg(const char *message);
