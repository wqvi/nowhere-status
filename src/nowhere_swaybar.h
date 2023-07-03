#ifndef NOWHERE_SWAYBAR_H
#define NOWHERE_SWAYBAR_H

#include "nowhere_map.h"

struct nowhere_color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	char _unused;
};

struct nowhere_node {
	char name[16];
	char full_text[48];
	struct nowhere_color color;
};

struct nowhere_swaybar {
	int timerfd;
	int epollfd;
	nowhere_map_t map;
};

int nowhere_swaybar_create(struct nowhere_swaybar *_swaybar);

int nowhere_swaybar_start(struct nowhere_swaybar *_swaybar);

void nowhere_swaybar_destroy(struct nowhere_swaybar *_swaybar);

#endif // NOWHERE_SWAYBAR_H
