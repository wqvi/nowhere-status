#ifndef NOWHERE_SWAYBAR_H
#define NOWHERE_SWAYBAR_H

#include <curl/curl.h>
#include "map.h"

#ifndef NOWHERE_NAMSIZ
#define NOWHERE_NAMSIZ 16
#endif

#ifndef NOWHERE_TXTSIZ
#define NOWHERE_TXTSIZ 32
#endif

struct nowhere_config {
	int offline;
	int zone;
	char ifname[16];
	char location[16];
};

struct nowhere_color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	char _unused;
};

struct nowhere_node {
	char name[NOWHERE_NAMSIZ];
	char full_text[NOWHERE_TXTSIZ];
	char alt_text[NOWHERE_TXTSIZ];
	struct nowhere_color color;
	struct nowhere_node *next;
	int usage;
};

struct nowhere_swaybar {
	int timerfd;
	int weatherfd;
	int epollfd;
	struct nowhere_map *map;
	CURL *curl;
	struct nowhere_config config;
};

int nowhere_swaybar_create(struct nowhere_swaybar *_swaybar, struct nowhere_config *_config);

int nowhere_swaybar_start(struct nowhere_swaybar *_swaybar);

void nowhere_swaybar_destroy(struct nowhere_swaybar *_swaybar);

#endif // NOWHERE_SWAYBAR_H
