#include "map.h"
#include "swaybar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t bezout_hash(const char *_key) {
	size_t hash = 2;
	while (*_key) {
		hash = (hash * 3) ^ (_key[0] * 5);
		_key++;
	}
	return hash;
}

static void quadratic_index(size_t *_index, struct nowhere_node *_entries, size_t _count, const char *_key) {
	struct nowhere_node *current = &_entries[*_index];
	int i = 0;

	while (current->name[0] && strcmp(_key, current->name) != 0) {
		*_index = (*_index + i * i) % _count;
		i++;
		current = &_entries[*_index]; 
	}
}

int nowhere_map_create(struct nowhere_map **_map, struct node_info *_infos, size_t _count) {
	size_t size = sizeof(struct nowhere_map) + _count * sizeof(struct nowhere_node);
	void *ptr;
	if (!(ptr = calloc(1, size))) return -1;
	memset(ptr, 0, size);

	struct nowhere_map *map = ptr;
	map->count = _count;
	map->entries = (struct nowhere_node *)(map + 1);

	for (int i = 0; i < _count; i++) {
		struct node_info *info = &_infos[i];
		struct nowhere_node *node = &map->entries[i];
		struct nowhere_node *next = &map->entries[i + 1];
		if (i == _count - 1) next = NULL;

		node->flags = info->flags;
		node->next = next;
		memcpy(node->name, info->name, NOWHERE_NAMSIZ);
	}

	*_map = map;
	
	return 0;
}

void nowhere_map_put(struct nowhere_map *_map, struct nowhere_node *_node) {
	struct nowhere_node *head = _map->entries;
	while (head != NULL) {
		if (strcmp(head->name, _node->name) == 0) {
			if (head->flags & NOWHERE_NODE_DEFAULT) memcpy(head->full_text, _node->full_text, NOWHERE_TXTSIZ);
			if (head->flags & NOWHERE_NODE_COLOR) memcpy(&head->color, &_node->color, sizeof(struct color));
			if (head->flags & NOWHERE_NODE_ALT) memcpy(head->alt_text, _node->alt_text, NOWHERE_TXTSIZ);
			break;
		}
		head = head->next;
	}
}

struct nowhere_node *nowhere_map_get(struct nowhere_map *_map, const char *_name) {
	struct nowhere_node *head = _map->entries;
	while (head != NULL) {
		if (strcmp(head->name, _name) == 0) {
			return head;
		}
		head = head->next;
	}
	return NULL;
}

void nowhere_map_print(struct nowhere_map *_map) {
	printf(",[");
	struct nowhere_node *head = _map->entries;
	while (head != NULL) {
		printf("{");
		printf("\"name\":\"%s\",", head->name);
		if (head->flags & NOWHERE_NODE_ALT) printf("\"full_text\":\"%s\"", head->alt_text); 
		else printf("\"full_text\":\"%s\",", head->full_text);
		if (head->flags & NOWHERE_NODE_COLOR) printf("\"color\":\"#%02x%02x%02x\",", head->color.r, head->color.g, head->color.b);
		printf("}");

		if (head->next) printf(",");

		head = head->next;
	}
	printf("]\n");
	fflush(stdout);
}
