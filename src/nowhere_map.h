#ifndef NOWHERE_MAP_H
#define NOWHERE_MAP_H

#include <stddef.h>

typedef struct nowhere_map *nowhere_map_t;

int nowhere_map_create(nowhere_map_t *_map, size_t _key_count, size_t _entry_size);

void nowhere_map_put(nowhere_map_t _map, void *_key, const void *_data);

void *nowhere_map_get(nowhere_map_t _map, const void *_key);

#endif //NOWHERE_MAP_H
