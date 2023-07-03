#include "nowhere_alloc.h"

#include <stdlib.h>

typedef uint16_t offset_t;

void *nowhere_alloc(size_t _align, size_t _size) {
        if (!_align || !_size) return NULL;

        if (!NOWHERE_BASE_2(_align) || NOWHERE_ALIGNED(_size, _align)) return NULL;

        char *unaligned_ptr = malloc(_size + sizeof(offset_t) + (_align - 1));
        if (!unaligned_ptr) return NULL;

        void *ptr = (void *)NOWHERE_ALIGN((uintptr_t)(unaligned_ptr + sizeof(offset_t)), _align);
        // Store difference of aligned and unaligned pointer
        // Used for retreiving original pointer address upon freeing
        uintptr_t diff = (uintptr_t)ptr - (uintptr_t)unaligned_ptr;
        *((offset_t *)ptr - 1) = (offset_t)diff;

        // DO NOT FREE THIS POINTER
        return ptr;
}


void nowhere_free(void *_ptr) {
        if (!_ptr) return;

        // Retrieve difference of aligned an unaligned pointer
        // Subtract this from the _ptr variable to get the original allocated address
        offset_t offset = *((offset_t *)_ptr - 1);

        void *head = ((char *)_ptr) - offset;

        free(head);
}
