/*
	*	MIT License
	*
	*	Copyright (c) 2025 xyurt
	*
	*	Permission is hereby granted, free of charge, to any person obtaining a copy
	*	of this software and associated documentation files (the "Software"), to deal
	*	in the Software without restriction, including without limitation the rights
	*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	*	copies of the Software, and to permit persons to whom the Software is
	*	furnished to do so, subject to the following conditions:
	*
	*	The above copyright notice and this permission notice shall be included in all
	*	copies or substantial portions of the Software.
	*
	*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	*	SOFTWARE.
*/

#include "vec89.h"

#include <stdlib.h>
#include <string.h>

#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define MALLOC_FUNCTION(Size) malloc(Size)
#define REALLOC_FUNCTION(Block, Size) realloc(Block, Size)

char VEC89_INITIALIZATION(vec_p vec, size_t element_size) {
	if (vec == NULL || element_size == 0 || VEC89_DEFAULT_CAPACITY < 0) return VEC89_INVALID_ARGUMENTS;

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK_TYPE *vec89_lock = malloc(sizeof(VEC89_LOCK_TYPE));
	if (vec89_lock == NULL) return VEC89_MEMORY_ERROR;

	VEC89_LOCK_INIT(vec89_lock);
	VEC89_LOCK(vec89_lock);
	vec->lock = vec89_lock;
#endif

	void *arr_block = MALLOC_FUNCTION(element_size * VEC89_DEFAULT_CAPACITY);
	if (arr_block == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_LOCK_DESTROY(vec->lock);
		free(vec->lock);
		vec->lock = NULL;
#endif
		return VEC89_MEMORY_ERROR;
	}

	vec->arr = arr_block;
	vec->capacity = VEC89_DEFAULT_CAPACITY;
	vec->elem_size = element_size;
	vec->count = 0;

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif
	
	return VEC89_SUCCESS;
}

void VEC89_ARRAY_FREE(vec_p vec) {
	if (vec == NULL) return;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	free(vec->arr);
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif
	return;
}

void VEC89_FREE(vec_p vec) {
	if (vec == NULL) return;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	free(vec->arr);
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK_TYPE *lock_pointer = vec->lock;
#endif
	free(vec);
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK_DESTROY(lock_pointer);
	free(lock_pointer);
#endif
	return;
}

char VEC89_CLEAR(vec_p vec) {
	if (vec == NULL) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	vec->count = 0;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif
	return VEC89_SUCCESS;
}

char VEC89_RESERVE(vec_p vec, size_t capacity) {
	if (vec == NULL || capacity == 0) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL || capacity < vec->count) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	if (capacity == vec->capacity) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_SUCCESS;
	}

	void *arr_block = REALLOC_FUNCTION(vec->arr, vec->elem_size * capacity);
	if (arr_block == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_MEMORY_ERROR;
	}

	vec->arr = arr_block;
	vec->capacity = capacity;

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif

	return VEC89_SUCCESS;
}

char VEC89_EXPAND(vec_p vec, size_t n) {
	if (vec == NULL) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	if (n == 0) return VEC89_SUCCESS;

	size_t target_capacity = vec->capacity << n;

	void *arr_block = REALLOC_FUNCTION(vec->arr, vec->elem_size * target_capacity);
	if (arr_block == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_MEMORY_ERROR;
	}

	vec->arr = arr_block;
	vec->capacity = target_capacity;

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif

	return VEC89_SUCCESS;
}

char VEC89_SHRINK(vec_p vec, size_t n) {
	if (vec == NULL) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	if (n == 0 || vec->capacity <= 1 || vec->capacity <= vec->count) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_SUCCESS;
	}

	size_t target_capacity = vec->capacity;
	size_t i;
	for (i = 0; i < n; i++) {
		if (target_capacity <= 1 || target_capacity <= vec->count) break;
		target_capacity /= 2;
	}

	void *arr_block = REALLOC_FUNCTION(vec->arr, vec->elem_size * target_capacity);
	if (arr_block == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_MEMORY_ERROR;
	}

	vec->arr = arr_block;
	vec->capacity = target_capacity;

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif

	return VEC89_SUCCESS;
}

char VEC89_SHRINK_TO_FIT(vec_p vec) {
	if (vec == NULL) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	if (vec->capacity == 0 || vec->capacity == vec->count) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_SUCCESS;
	}

	void *arr_block = REALLOC_FUNCTION(vec->arr, vec->elem_size * max(vec->count, 1));
	if (arr_block == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_MEMORY_ERROR;
	}

	vec->arr = arr_block;
	vec->capacity = max(vec->count, 1);

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif

	return VEC89_SUCCESS;
}

char VEC89_PUSH(vec_p vec, const void *element) {
	if (vec == NULL || element == NULL ) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	
	if (vec->count >= vec->capacity) {
		size_t target_capacity = vec->capacity * 2;

		void *arr_block = REALLOC_FUNCTION(vec->arr, vec->elem_size * target_capacity);
		if (arr_block == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
			VEC89_UNLOCK(vec->lock);
#endif
			return VEC89_MEMORY_ERROR;
		}

		vec->arr = arr_block;
		vec->capacity = target_capacity;
	}

	memcpy(vec->arr + vec->count * vec->elem_size, element, vec->elem_size);
	vec->count++;

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif

	return VEC89_SUCCESS;
}

char VEC89_POP(vec_p vec, void **out_element) {
	if (vec == NULL || out_element == NULL) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	if (vec->count == 0) {
		*out_element = NULL;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_SUCCESS;
	}

	*out_element = MALLOC_FUNCTION(vec->elem_size);
	if (*out_element == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_MEMORY_ERROR;
	}

	memcpy(*out_element, vec->arr + vec->elem_size * (vec->count - 1), vec->elem_size);
	vec->count--;

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif

	return VEC89_SUCCESS;
}

char VEC89_SET(vec_p vec, size_t idx, const void *element) {
	if (vec == NULL || element == NULL) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	if (idx >= vec->capacity) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_ARRAY_OUT_OF_INDEX;
	}

	memcpy(vec->arr + vec->elem_size * idx, element, vec->elem_size);

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif

	return VEC89_SUCCESS;
}

char VEC89_REMOVE(vec_p vec, size_t idx) {
	if (vec == NULL) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	if (idx >= vec->count) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_ARRAY_OUT_OF_INDEX;
	}

	if (idx == vec->count - 1) {
		vec->count--;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_SUCCESS;
	}

	memmove(vec->arr + vec->elem_size * idx, vec->arr + vec->elem_size * (idx + 1), vec->elem_size * (vec->count - (idx + 1)));
	vec->count--;

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif

	return VEC89_SUCCESS;
}

char VEC89_INSERT(vec_p vec, size_t idx, const void *element) {
	if (vec == NULL || element == NULL) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	if (idx > vec->count) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_ARRAY_OUT_OF_INDEX;
	}

	if (vec->count >= vec->capacity) {
		size_t target_capacity = vec->capacity * 2;

		void *arr_block = REALLOC_FUNCTION(vec->arr, vec->elem_size * target_capacity);
		if (arr_block == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
			VEC89_UNLOCK(vec->lock);
#endif
			return VEC89_MEMORY_ERROR;
		}

		vec->arr = arr_block;
		vec->capacity = target_capacity;
	}

	if (idx >= vec->count) {
		memcpy(vec->arr + vec->elem_size * vec->count, element, vec->elem_size);
		vec->count++;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_SUCCESS;
	}
	
	memmove(vec->arr + vec->elem_size * (idx + 1), vec->arr + vec->elem_size * idx, vec->elem_size * (vec->count - idx));
	memcpy(vec->arr + vec->elem_size * idx, element, vec->elem_size);

	vec->count++;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif
	return VEC89_SUCCESS;
}

char VEC89_GET(vec_p vec, size_t idx, void **out_element) {
	if (vec == NULL || out_element == NULL) return VEC89_INVALID_ARGUMENTS;
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK(vec->lock);
#endif
	if (vec->arr == NULL) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
			VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_INVALID_ARGUMENTS;
	}
	if (idx >= vec->count) {
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
		VEC89_UNLOCK(vec->lock);
#endif
		return VEC89_ARRAY_OUT_OF_INDEX;
	}

	*out_element = vec->arr + vec->elem_size * idx;

#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_UNLOCK(vec->lock);
#endif
	return VEC89_SUCCESS;
}