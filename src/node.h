#ifndef NOWHERE_MAP_H
#define NOWHERE_MAP_H

#include <stddef.h>

struct node;

struct node_info;

int llist_create(struct node **_head, struct node_info *_infos, size_t _count);

void llist_put(struct node *_head, struct node *_node);

struct node *llist_get(struct node *_head, const char *_name);

void llist_print(struct node *_head);

#endif //NOWHERE_MAP_H
