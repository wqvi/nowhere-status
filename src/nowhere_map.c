#include "nowhere_map.h"
#include "nowhere_swaybar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void nowhere_print_node(struct nowhere_node *_node) {
	printf("{");
	printf("\"name\":\"%s\",", _node->name);
	if (_node->color._unused) {
		printf("\"color\":\"#%02x%02x%02x\",", _node->color.r, _node->color.g, _node->color.b);
	}
	if (!_node->usage) {
		printf("\"full_text\":\"%s\"", _node->full_text);
	} else {
		printf("\"full_text\":\"%s\"", _node->alt_text);
	}
	printf("}");
	if (_node->next != NULL) printf(",");
}

static size_t nowhere_bezout_hash(const char *_key) {
	size_t hash = 2;
	while (*_key) {
		hash = (hash * 3) ^ (_key[0] * 5);
		_key++;
	}
	return hash;
}

int nowhere_map_create(struct nowhere_map **_map, size_t _count) {
	size_t size = sizeof(struct nowhere_map) + _count * sizeof(struct nowhere_node);

	char *ptr = malloc(size);
	if (!ptr) return -1;

	memset(ptr, 0, size);

	struct nowhere_map *map = (struct nowhere_map *)ptr;
	map->count = _count;
	map->entries = (struct nowhere_node *)(ptr + sizeof(struct nowhere_map));

	*_map = map;

	return 0;
}

struct nowhere_node *nowhere_map_put(struct nowhere_map *_map, struct nowhere_node *_node) {
	size_t hash = nowhere_bezout_hash(_node->name);
	size_t index = (hash % _map->count);
	
	struct nowhere_node *node = &_map->entries[index];

	if (strcmp(_node->name, node->name) == 0) {
		int usage = node->usage;
		struct nowhere_node *next = node->next;
		memcpy(node, _node, sizeof(struct nowhere_node));
		node->usage = usage;
		node->next = next;
		return node;
	}

	int i = 1;
	while (_map->entries[index].name[0] && strcmp(_node->name, _map->entries[index].name) != 0) {
		index = (index + i * i) % _map->count;
		i++;
	}

	int usage = _map->entries[index].usage;
	struct nowhere_node *next = _map->entries[index].next;
	memcpy(&_map->entries[index], _node, sizeof(struct nowhere_node));
	_map->entries[index].usage = usage;
	_map->entries[index].next  = next;

	return &_map->entries[index];
}

struct nowhere_node *nowhere_map_get(struct nowhere_map *_map, const char *_name) {
	size_t hash = nowhere_bezout_hash(_name);
	size_t index = (hash % _map->count);

	struct nowhere_node *node = &_map->entries[index];

	if (strcmp(_name, node->name) == 0) {
		return node;
	}

	int i = 1;
	while (_map->entries[index].name[0] && strcmp(_name, _map->entries[index].name) != 0) {
		index = (index + i * i) % _map->count;
		i++;
	}

	return &_map->entries[index];
}

void nowhere_map_print(struct nowhere_map *_map) {
	printf(",[");
	struct nowhere_node *head = _map->head;
	while (head != NULL) {
		nowhere_print_node(head);
		head = head->next;
	}
	printf("]\n");
	fflush(stdout);
}
