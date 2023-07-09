#include "map.h"
#include "swaybar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int nowhere_map_create(struct node **_head, struct node_info *_infos, size_t _count) {
	size_t size = _count * sizeof(struct node);
	void *ptr;
	if (!(ptr = malloc(size))) return -1;

	for (int i = 0; i < _count; i++) {
		struct node_info *info = &_infos[i];
		struct node *node = (struct node *)ptr + i;
		struct node *next = node + 1;
		if (i == _count - 1) next = NULL;

		node->flags = info->flags & ~NOWHERE_NODE_ALT;
		node->next = next;
		node->fun = info->fun;
		memcpy(node->name, info->name, NOWHERE_NAMSIZ);
	}

	*_head = ptr;
	
	return 0;
}

void nowhere_map_put(struct node *_head, struct node *_node) {
	struct node *head = _head;
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

struct node *nowhere_map_get(struct node *_head, const char *_name) {
	struct node *head = _head;
	while (head != NULL) {
		if (strcmp(head->name, _name) == 0) {
			return head;
		}
		head = head->next;
	}
	return NULL;
}

void nowhere_map_print(struct node *_head) {
	printf(",[");
	struct node *head = _head;
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
