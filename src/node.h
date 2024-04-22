#ifndef NOWHERE_MAP_H
#define NOWHERE_MAP_H

#include <stddef.h>

struct node;

struct node_info;

int llist_create(struct node **_head, struct node_info *_infos, size_t _count);

struct node *llist_get(struct node *_head, char _name);

void llist_print(struct node *_head);

#endif //NOWHERE_MAP_H
