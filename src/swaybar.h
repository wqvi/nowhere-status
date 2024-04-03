#ifndef NOWHERE_SWAYBAR_H
#define NOWHERE_SWAYBAR_H

#include "node.h"

#ifndef NOWHERE_NAMSIZ
#define NOWHERE_NAMSIZ 16
#endif

#ifndef NOWHERE_TXTSIZ
#define NOWHERE_TXTSIZ 48
#endif

struct color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char _unused;
};

enum node_flags {
	NOWHERE_NODE_DEFAULT = 0x1,
	NOWHERE_NODE_COLOR = 0x2,
	NOWHERE_NODE_ALT = 0x4
};

struct node;

typedef int (*update)(struct node *);

struct node_info {
	int flags;
	char name[NOWHERE_NAMSIZ];
	update fun;
};

struct node {
	int flags;
	struct node *next;
	update fun;
	char name[NOWHERE_NAMSIZ];
	char full_text[NOWHERE_TXTSIZ];
	struct color color;
	char alt_text[NOWHERE_TXTSIZ];
};

struct nowhere_swaybar {
	int timerfd;
	int epollfd;
	struct node *head;
};

int swaybar_create(struct nowhere_swaybar **_swaybar);

int swaybar_start(struct nowhere_swaybar *_swaybar);

void swaybar_destroy(struct nowhere_swaybar *_swaybar);

#endif // NOWHERE_SWAYBAR_H
