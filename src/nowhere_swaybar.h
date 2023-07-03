#ifndef NOWHERE_SWAYBAR_H
#define NOWHERE_SWAYBAR_H

#include <curl/curl.h>
#include "nowhere_map.h"

#ifndef NOWHERE_NAMSIZ
#define NOWHERE_NAMSIZ 16
#endif

#ifndef NOWHERE_TXTSIZ
#define NOWHERE_TXTSIZ 32
#endif

struct nowhere_color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	char _unused;
};

enum nowhere_usage {
	NOWHERE_DEFAULT = 0x1,
	NOWHERE_ALT = 0x2,
	NOWHERE_DISABLED = 0x4
};

struct nowhere_node {
	char name[NOWHERE_NAMSIZ];
	char full_text[NOWHERE_TXTSIZ];
	char alt_text[NOWHERE_TXTSIZ];
	struct nowhere_color color;
	int usage;
};

struct nowhere_swaybar {
	int timerfd;
	int weatherfd;
	int epollfd;
	nowhere_map_t map;
	CURL *curl;
};

int nowhere_swaybar_create(struct nowhere_swaybar *_swaybar);

int nowhere_swaybar_start(struct nowhere_swaybar *_swaybar);

void nowhere_swaybar_destroy(struct nowhere_swaybar *_swaybar);

#endif // NOWHERE_SWAYBAR_H
