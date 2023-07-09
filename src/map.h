#ifndef NOWHERE_MAP_H
#define NOWHERE_MAP_H

#include <stddef.h>

struct node;

struct node_info;

int nowhere_map_create(struct node **_head, struct node_info *_infos, size_t _count);

void nowhere_map_put(struct node *_head, struct node *_node);

struct node *nowhere_map_get(struct node *_head, const char *_name);

void nowhere_map_print(struct node *_head);

#endif //NOWHERE_MAP_H
