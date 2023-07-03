#ifndef NOWHERE_MAP_H
#define NOWHERE_MAP_H

#include <stddef.h>

struct nowhere_node;

typedef struct nowhere_map *nowhere_map_t;

int nowhere_map_create(nowhere_map_t *_map, size_t _count);

void nowhere_map_put(nowhere_map_t _map, struct nowhere_node *_node);

struct nowhere_node *nowhere_map_get(nowhere_map_t _map, const char *_name);

void nowhere_map_print(nowhere_map_t _map);

#endif //NOWHERE_MAP_H
