#include "node.h"
#include "swaybar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int llist_create(struct node **_head, struct node_info *_infos, size_t _count) {
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
		node->name = info->name;
	}

	*_head = ptr;
	
	return 0;
}

struct node *llist_get(struct node *_head, char _name) {
	struct node *head = _head;
	while (head != NULL) {
		if (head->name == _name) {
			return head;
		}

		head = head->next;
	}
	return NULL;
}

void llist_print(struct node *_head) {
	printf(",[");
	struct node *head = _head;
	while (head != NULL) {
		printf("{");
		printf("\"name\":\"%c\",", head->name);

		if (head->flags & NOWHERE_NODE_ALT) {
			printf("\"full_text\":\"%s\"", head->alt_text); 
		} else {
			printf("\"full_text\":\"%s\",", head->full_text);
		}

		if (head->flags & NOWHERE_NODE_COLOR) {
			printf("\"color\":\"#%02x%02x%02x\",", head->color.r, head->color.g, head->color.b);
		}

		printf("}");

		if (head->next) {
			printf(",");
		}

		head = head->next;
	}

	printf("]\n");
	fflush(stdout);
}
