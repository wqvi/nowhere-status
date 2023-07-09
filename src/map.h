#ifndef NOWHERE_MAP_H
#define NOWHERE_MAP_H

#include <stddef.h>

struct node;

struct node_info;

struct nowhere_map {
	size_t count;
	struct node *entries;
};

int nowhere_map_create(struct nowhere_map **_map, struct node_info *_infos, size_t _count);

void nowhere_map_put(struct nowhere_map *_map, struct node *_node);

struct node *nowhere_map_get(struct nowhere_map *_map, const char *_name);

void nowhere_map_print(struct nowhere_map *_map);

#endif //NOWHERE_MAP_H
