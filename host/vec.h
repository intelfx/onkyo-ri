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
 * This file declares a simple pure-C "vector".
 */

#pragma once

#define _GNU_SOURCE
#include <stdlib.h>

struct vec
{
	void *data;
	size_t size, capacity;
};

struct vec vec_new(void);
void vec_delete(struct vec *vec);
void *vec_end(const struct vec *vec);
size_t vec_headroom(const struct vec *vec);
int vec_double(struct vec *vec);
int vec_add_one(struct vec *vec, char c);

inline static void vec_deletep(struct vec **vecp)
{
	if (vecp != NULL) {
		vec_delete(*vecp);
	}
}
#define _cleanup_vec_ __attribute__((cleanup(vec_deletep)))
