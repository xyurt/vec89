#include <stdio.h>
#include "include/vec89.h"

int main() {
    vec my_vec;

    // Initialize vector for int64_t elements
    if (vec_init(my_vec, sizeof(int)) != VEC89_SUCCESS) {
        printf("Failed to initialize vector\n");
        return 1;
    }

    // Push some values
    size_t i;
    for (i = 0; i < 10; i++) {
        if (vec_push(my_vec, &i) != VEC89_SUCCESS) {
            printf("Failed to push element %zu\n", i);
            break;
        }
    }

    // Print vector contents
    printf("Vector elements:\n");
    for (i = 0; i < my_vec.count; i++) {
        int *val;
        if (vec_get(my_vec, i, (void **)&val) == VEC89_SUCCESS) {
            printf("Element %zu: %d\n", i, *val);
        }
    }

    // Clear vector
    vec_clear(my_vec);

    // Shrink capacity to fit count (which is zero now)
    vec_shrink_to_fit(my_vec);

    // Free internal array
    vec_array_free(my_vec);

    return 0;
}
