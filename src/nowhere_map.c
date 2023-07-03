#include "nowhere_alloc.h"
#include "nowhere_map.h"
#include "nowhere_swaybar.h"
#include <stdio.h>
#include <string.h>

struct nowhere_map {
	size_t count;
	struct nowhere_node *entries;
};

static void nowhere_print_node(struct nowhere_node *_node) {
	printf("{");
	printf("\"name\":\"%s\",", _node->name);
	if (_node->color._unused) {
		printf("\"color\":\"#%02x%02x%02x\",", _node->color.r, _node->color.g, _node->color.b);
	}
	if (!_node->alt) {
		printf("\"full_text\":\"%s\"", _node->full_text);
	} else {
		printf("\"full_text\":\"%s\"", _node->alt_text);
	}
	printf("}");
}

static size_t nowhere_bezout_hash(const char *_key) {
	size_t hash = 2;
	while (*_key) {
		hash = (hash * 3) ^ (_key[0] * 5);
		_key++;
	}
	return hash;
}

int nowhere_map_create(nowhere_map_t *_map, size_t _count) {
	size_t entry_size = NOWHERE_ALIGN(sizeof(struct nowhere_node), 64);
	size_t size = sizeof(struct nowhere_map) + _count * entry_size;

	char *ptr = nowhere_alloc(entry_size, size);
	if (!ptr) return -1;

	memset(ptr, 0, size);

	nowhere_map_t map = (nowhere_map_t)ptr;
	map->count = _count;
	map->entries = (struct nowhere_node *)(ptr + sizeof(struct nowhere_map));

	*_map = map;

	return 0;
}

void nowhere_map_put(nowhere_map_t _map, struct nowhere_node *_node) {
	size_t hash = nowhere_bezout_hash(_node->name);
	size_t index = (hash % _map->count);
	
	struct nowhere_node *node = &_map->entries[index];

	if (strcmp(_node->name, node->name) == 0) {
		memcpy(node, _node, sizeof(struct nowhere_node) - sizeof(int));
		return;
	}

	int i = 1;
	while (_map->entries[index].name[0] && strcmp(_node->name, _map->entries[index].name) != 0) {
		index = (index + i * i) % _map->count;
		i++;
	}

	memcpy(&_map->entries[index], _node, sizeof(struct nowhere_node) - sizeof(int));
}

struct nowhere_node *nowhere_map_get(nowhere_map_t _map, const char *_name) {
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

void nowhere_map_print(nowhere_map_t _map) {
	printf(",[");
	for (int i = 0; i < _map->count; i++) {
		nowhere_print_node(&_map->entries[i]);
		if (i != _map->count - 1) {
			printf(",");
		}
	}
	printf("]\n");
	fflush(stdout);
}
