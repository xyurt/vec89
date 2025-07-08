# vec89

**vec89** is a lightweight, generic, and optionally thread-safe dynamic array (vector) library written in C89-compatible style. It provides a simple and efficient API for managing dynamic arrays of any data type with minimal dependencies.

---

## Features

- Generic dynamic array for any element type (using `void*` internally)
- Automatic resizing (expand, shrink, reserve, shrink-to-fit)
- Push, pop, insert, remove, set, and get operations
- Optional thread safety with platform-specific locks:
  - Windows: `CRITICAL_SECTION`
  - POSIX: `pthread_mutex_t`
- Minimal dependencies, easy to embed in any C project
- Designed for C89 compatibility and portability

---

## Getting Started

### Integration

1. Copy `vec89.h` and `vec89.c` into your project.
2. Include `vec89.h` in your source files:

```c
#include "vec89.h"
```

---

### Basic Usage

```c
#include <stdio.h>
#include "vec89.h"

int main() {
    vec my_vec;

    if (VEC89_INITIALIZATION(&my_vec, sizeof(int)) != VEC89_SUCCESS) {
        printf("Failed to initialize vector\n");
        return 1;
    }

    int value = 42;
    VEC89_PUSH(&my_vec, &value);

    int *out_val;
    if (VEC89_GET(&my_vec, 0, (void **)&out_val) == VEC89_SUCCESS) {
        printf("First element: %d\n", *out_val);
    }

    VEC89_ARRAY_FREE(&my_vec);

    return 0;
}
```

---

## API Summary

| Function                 | Description                                         |
|--------------------------|-----------------------------------------------------|
| `VEC89_INITIALIZATION`    | Initialize vector with element size                  |
| `VEC89_ARRAY_FREE`        | Free internal data array                             |
| `VEC89_FREE`              | Free vector and internal data (heap-allocated only)|
| `VEC89_CLEAR`             | Clear vector (count = 0)                            |
| `VEC89_RESERVE`           | Reserve capacity                                    |
| `VEC89_EXPAND`            | Expand capacity by doubling n times                 |
| `VEC89_SHRINK`            | Shrink capacity by dividing by two up to n times   |
| `VEC89_SHRINK_TO_FIT`     | Shrink capacity to current element count            |
| `VEC89_PUSH`              | Append element to the end                            |
| `VEC89_POP`               | Remove and retrieve last element                     |
| `VEC89_SET`               | Set element at given index                           |
| `VEC89_REMOVE`            | Remove element at given index and shift remaining   |
| `VEC89_INSERT`            | Insert element at given index and shift             |
| `VEC89_GET`               | Retrieve pointer to element at given index          |

---

## Thread Safety

To enable thread safety:

- Define `VEC89_THREAD_SAFE_IMPLEMENTATION_NOTC89` in the header file or globally.
- On Windows, the library uses `CRITICAL_SECTION`.
- On POSIX systems, it uses `pthread_mutex_t`.
- Lock is allocated dynamically inside the vector struct and must be destroyed and freed manually when done.

---

## Notes and Caveats

- When using thread safety, remember to destroy and free the lock manually if you use stack-allocated vectors.
- `VEC89_POP` returns a dynamically allocated copy of the popped element; caller is responsible for freeing it.
- `VEC89_FREE` should only be used if the vector itself was heap allocated.
- This library is designed with minimal C89 compatibility and portability in mind; it avoids C99+ features.

---

## Contributing

Contributions, issues, and feature requests are welcome! Feel free to fork and submit pull requests.
