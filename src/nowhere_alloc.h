#ifndef NOWHERE_ALLOC_H
#define NOWHERE_ALLOC_H

#include <stddef.h>
#include <stdint.h>

#ifndef NOWHERE_ALIGN
#define NOWHERE_ALIGN(unaligned, alignment) \
    (((unaligned) + ((alignment) - 1)) & ~((alignment) - 1))
#endif // NOWHERE_ALIGN

#ifndef NOWHERE_BASE_2
#define NOWHERE_BASE_2(item) (((item) & ((item) - 1)) == 0)
#endif // NOWHERE_BASE_2

#ifndef NOWHERE_ALIGNED
#define NOWHERE_ALIGNED(item, alignment) ((NOWHERE_ALIGN((item), (alignment)) - (item)) == 0 && NOWHERE_BASE_2((item)))
#endif // NOWHERE_ALIGNED

void *nowhere_alloc(size_t _align, size_t _size);

void nowhere_free(void *_ptr);
#endif // NOWHERE_ALLOC_H
