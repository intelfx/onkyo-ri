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
 * This file implements a simple pure-C "vector".
 */

#include "vec.h"

struct vec vec_new(void)
{
#define VEC_ALLOC_BASE 8
	struct vec vec = {
		.data = malloc(VEC_ALLOC_BASE),
		.size = 0,
		.capacity = VEC_ALLOC_BASE
	};

	if (vec.data == NULL) {
		vec.capacity = 0;
	}

	return vec;
}

void vec_delete(struct vec *vec)
{
	free(vec->data);
	vec->data = NULL;
	vec->size = 0;
	vec->capacity = 0;
}

void *vec_end(const struct vec *vec)
{
	return (char *)vec->data + vec->size;
}

size_t vec_headroom(const struct vec *vec)
{
	return vec->capacity - vec->size;
}

int vec_double(struct vec *vec)
{
	void *vec_new_data = realloc(vec->data, vec->capacity * 2);
	if (vec_new_data == NULL) {
		return -1;
	}
	vec->data = vec_new_data;
	vec->capacity *= 2;
	return 0;
}

int vec_add_one(struct vec *vec, char c)
{
	int r;

	while (vec_headroom(vec) < 1) {
		r = vec_double(vec);
		if (r < 0) {
			return r;
		}
	}

	*((char *)vec->data + vec->size++) = c;
	return 0;
}
