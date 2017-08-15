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
 * This file defines a few assorted helper macros.
 */

#pragma once

#include <string.h>

#define streq(_1, _2) (strcmp((_1), (_2)) == 0)
#define strprefix(str, prefix) (strncmp((str), (prefix), strlen((prefix))) == 0)

static inline void freep(void *pp)
{
	if (pp != NULL) {
		free(*(void **)pp);
	}
}
#define _cleanup_free_ __attribute__((cleanup(freep)))
