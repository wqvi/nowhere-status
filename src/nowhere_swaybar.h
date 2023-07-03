#ifndef NOWHERE_SWAYBAR_H
#define NOWHERE_SWAYBAR_H

struct nowhere_swaybar {
	int timerfd;
	int epollfd;
};

int nowhere_swaybar_create(struct nowhere_swaybar *_swaybar);

int nowhere_swaybar_start(struct nowhere_swaybar *_swaybar);

void nowhere_swaybar_destroy(struct nowhere_swaybar *_swaybar);

#endif // NOWHERE_SWAYBAR_H
