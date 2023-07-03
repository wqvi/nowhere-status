#include "nowhere_alloc.h"
#include "nowhere_map.h"
#include <string.h>

struct nowhere_map {
	size_t key_count;
	size_t entry_size;
	char *entries;
};

static size_t nowhere_map_murmurhash3(const void *_key) {
	uintptr_t key = (uintptr_t)_key;
	key ^= (key >> 33);
	key *= 0xFF51AFD7ED558CCD;
	key ^= (key >> 33);
	key *= 0xC4CEB9FE1A85EC53;
	key ^= (key >> 33);
	return key;
}

static size_t nowhere_bezout_hash(const char *_key) {
	size_t hash = 2;
	while (*_key) {
		hash = (hash * 3) ^ (*_key * 5);
		_key++;
	}
	return hash;
}

int nowhere_map_create(nowhere_map_t *_map, size_t _key_count, size_t _entry_size) {
	if (!NOWHERE_BASE_2(_entry_size)) return -1;
	size_t entry_size = NOWHERE_ALIGN(sizeof(void *) + _entry_size, _entry_size);
	size_t size = sizeof(struct nowhere_map) + _key_count * entry_size;

	char *ptr = nowhere_alloc(_entry_size, size);
	if (!ptr) return -1;

	memset(ptr, 0, size);

	nowhere_map_t map = (nowhere_map_t)ptr;
	map->key_count = _key_count;
	map->entry_size = _entry_size;
	map->entries = ptr + sizeof(struct nowhere_map);

	*_map = map;

	return 0;
}

void nowhere_map_put(nowhere_map_t _map, void *_key, const void *_data) {
	size_t entry_size = NOWHERE_ALIGN(sizeof(void *) + _map->entry_size, _map->entry_size);
	size_t hash = nowhere_map_murmurhash3(_key);
	size_t index = (hash % _map->key_count);
	size_t memory_index = index * entry_size;

	intptr_t *key_ptr = (intptr_t *)(_map->entries + memory_index);
	char *data_ptr = _map->entries + memory_index + entry_size - _map->entry_size;

	if ((void *)*key_ptr == _key) {
		memcpy(key_ptr, &_key, sizeof(void *));
		memcpy(data_ptr, _data, _map->entry_size);
		return;
	}

	int i = 1;
	while ((void *)*key_ptr && (void *)*key_ptr != _key) {
		index = (index + i * i) % _map->key_count;
		memory_index = index * entry_size;

		key_ptr = (intptr_t *)(_map->entries + memory_index);
		data_ptr = _map->entries + memory_index + entry_size - _map->entry_size;
		i++;
	}

	memcpy(key_ptr, &_key, sizeof(void *));
	memcpy(data_ptr, _data, _map->entry_size);
}

void *nowhere_map_get(nowhere_map_t _map, const void *_key) {
	size_t entry_size = NOWHERE_ALIGN(sizeof(void *) + _map->entry_size, _map->entry_size);
	size_t hash = nowhere_map_murmurhash3(_key);
	size_t index = (hash % _map->key_count);
	size_t memory_index = index * entry_size;

	intptr_t *key_ptr = (intptr_t *)(_map->entries + memory_index);
	char *data_ptr = _map->entries + memory_index + entry_size - _map->entry_size;

	if ((void *)*key_ptr == _key) {
		return data_ptr;
	}

	int i = 1;
	while ((void *)*key_ptr && (void *)*key_ptr != _key) {
		index = (index + i * i) % _map->key_count;
		memory_index = index * entry_size;

		key_ptr = (intptr_t *)(_map->entries + memory_index);
		data_ptr = _map->entries + memory_index + entry_size - _map->entry_size;
		i++;
	}

	return data_ptr;
}
