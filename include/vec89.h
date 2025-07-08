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

#ifndef VEC89_H
#define VEC89_H

#define VEC89_DEFAULT_CAPACITY 15 /* Starting capacity for newly initialized vectors */

#define VEC89_SUCCESS 0
#define VEC89_FAILURE 1
#define VEC89_MEMORY_ERROR 2
#define VEC89_INVALID_ARGUMENTS 3
#define VEC89_ARRAY_OUT_OF_INDEX 4

#include <stdio.h>

/*
#define VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89 
*/

#define VEC89_FUNCTION_MACROS

/* Define this in order to implement thread safety but not C89 */
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89 
	#ifdef _WIN32
		#include <windows.h>
		#define VEC89_LOCK_TYPE CRITICAL_SECTION
		#define VEC89_LOCK_INIT(lock_p) InitializeCriticalSection(lock_p)
		#define VEC89_LOCK(lock_p) EnterCriticalSection(lock_p)
		#define VEC89_UNLOCK(lock_p) LeaveCriticalSection(lock_p)
		#define VEC89_LOCK_DESTROY(lock_p) DeleteCriticalSection(lock_p)
	#else
		#include <pthread.h>
		#define VEC89_LOCK_TYPE pthread_mutex_t
		#define VEC89_LOCK_INIT(lock_p) pthread_mutex_init(lock_p, NULL)
		#define VEC89_LOCK(lock_p) pthread_mutex_lock(lock_p)
		#define VEC89_UNLOCK(lock_p) pthread_mutex_unlock(lock_p)
		#define VEC89_LOCK_DESTROY(lock_p) pthread_mutex_destroy(lock_p)
	#endif
#endif

typedef struct VEC89 {
	char *arr;		  /* Array */
	size_t capacity;  /* Element capacity */
	size_t elem_size; /* Element size */
	size_t count;	  /* Element count */
#ifdef VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89
	VEC89_LOCK_TYPE *lock;
#endif
} vec, *vec_p, vec89; 

#ifdef VEC89_FUNCTION_MACROS
	#define vec_init(vec_obj, element_size) VEC89_INITIALIZATION(&vec_obj, element_size)
	#define vec_array_free(vec_obj) VEC89_ARRAY_FREE(&vec_obj)
	#define vec_free(vec_obj) VEC89_FREE(&vec_obj)
	#define vec_clear(vec_obj) VEC89_CLEAR(&vec_obj)

	#define vec_reserve(vec_obj, new_capacity) VEC89_RESERVE(&vec_obj, new_capacity)
	#define vec_expand(vec_obj, amount) VEC89_EXPAND(&vec_obj, amount)
	#define vec_shrink(vec_obj, amount) VEC89_SHRINK(&vec_obj, amount)
	#define vec_shrink_to_fit(vec_obj) VEC89_SHRINK_TO_FIT(&vec_obj)

	#define vec_push(vec_obj, element_ptr) VEC89_PUSH(&vec_obj, element_ptr)
	#define vec_pop(vec_obj, out_ptr_ptr) VEC89_POP(&vec_obj, out_ptr_ptr)

	#define vec_get(vec_obj, idx, out_ptr_ptr) VEC89_GET(&vec_obj, idx, out_ptr_ptr)
	#define vec_set(vec_obj, idx, element_ptr) VEC89_SET(&vec_obj, idx, element_ptr)
	#define vec_insert(vec_obj, idx, element_ptr) VEC89_INSERT(&vec_obj, idx, element_ptr)
	#define vec_remove(vec_obj, idx) VEC89_REMOVE(&vec_obj, idx)
#endif

/*
Initializes a vector with the given element size and the default capacity value
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*size_t element_size: Size of a single element in bytes. (element_size > 0)
*/
char VEC89_INITIALIZATION(vec_p vec, size_t element_size);

/*
Frees the given vector's array.
The vector pointer isn't freed.

*vec_p vec: Pointer to the vector.
*/
void VEC89_ARRAY_FREE(vec_p vec);

/*
Frees the given vector and its array.
Do not use this on stack allocated vectors, Freeing stack memory is undefined behaviour!

*vec_p vec: Pointer to the vector.
*/
void VEC89_FREE(vec_p vec);

/*
Sets the count value to zero.

*vec_p vec: Pointer to the vector.
*/
char VEC89_CLEAR(vec_p vec);

/*
Increases or decreases the capacity of the array. The capacity can't be lower than the element count.
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*size_t capacity: Target capacity. (capacity >= count) 
*/
char VEC89_RESERVE(vec_p vec, size_t capacity);

/*
Expands the vector as if its capacity is full n times.
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*size_t n: Times to expand. (n > 0)
*/
char VEC89_EXPAND(vec_p vec, size_t n);

/*
Shrinks the vector's capacity by dividing by two for maximum of n times.
The shrinking process ends when the next target capacity is lower than the element count.
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*size_t n: Maximum times to shrink. (n > 0)
*/
char VEC89_SHRINK(vec_p vec, size_t n);

/*
Shrinks the vector's capacity exactly to the current count
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*/
char VEC89_SHRINK_TO_FIT(vec_p vec);

/*
Pushes a new element at the end of the vector, if the capacity is sufficient then the vector expands.
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*char *element: Pointer to the element. (element != NULL)
*/
char VEC89_PUSH(vec_p vec, const void *element);

/*
Removes the last element at the end of the vector. The element is duplicated, element must be freed by the caller.
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*char **out_element: Pointer to the pointer to the element. (out_element != NULL)
*/
char VEC89_POP(vec_p vec, void **out_element);

/*
Sets an element at the index. The index cannot be out of range of the vector. The element is duplicated.
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*size_t idx: Index to set the element (0 <= idx < capacity)
*char *element: Pointer to the element. (element != NULL)
*/
char VEC89_SET(vec_p vec, size_t idx, const void *element);

/*
Removes the element at index. The remaining elements are shifted.
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*size_t idx: Index to set the element (0 <= idx < count)
*/
char VEC89_REMOVE(vec_p vec, size_t idx);

/*
Insert an element at index. The remaining elements are shifted.
If there is no space available, the vector expands.
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*size_t idx: Index to set the element (0 <= idx <= count <= capacity)
*char *element: Pointer to the element. (element != NULL)
*/
char VEC89_INSERT(vec_p vec, size_t idx, const void *element);

/*
Gets the element at index. The element isn't duplicated, the pointer's ownership is still at the vector.
Returns 0 on success, non-zero error codes on failure.

*vec_p vec: Pointer to the vector. (vec != NULL)
*size_t idx: Index to set the element (0 <= idx < count)
* *char **out_element: Pointer to the pointer to the element. (out_element != NULL)
*/
char VEC89_GET(vec_p vec, size_t idx, void **out_element);

#endif /* VEC89_H */