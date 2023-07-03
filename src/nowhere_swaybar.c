#include "nowhere_status.h"

#define _GNU_SOURCE
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

int nowhere_swaybar_create(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) return -1;

	struct timespec now;
	if (clock_gettime(CLOCK_REALTIME, &now) == -1) return -1;

	struct itimerspec timerspec = {
		.it_interval = { .tv_sec = 5, .tv_nsec = 0 },
		.it_value = { .tv_sec = now.tv_sec, .tv_nsec = 0 }
	};

	int timerfd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK);
	if (timerfd == -1) return -1;

	if (timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &timerspec, NULL) == -1) {
		close(timerfd);
		return -1;
	}

	int epollfd = epoll_create1(EPOLL_CLOEXEC);
	if (epollfd == -1) {
		close(timerfd);
		return -1;
	}

	struct epoll_event stdin_event = {
		.events = EPOLLIN,
		.data = { .fd = STDIN_FILENO }
	};

	struct epoll_event timer_event = {
		.events = EPOLLIN | EPOLLET,
		.data = { .fd = timerfd }
	};

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event) < 0) {
		close(timerfd);
		close(epollfd);
		return -1;
	}

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &timer_event) < 0) {
		close(timerfd);
		close(epollfd);
		return -1;
	}

	_swaybar->timerfd = timerfd;
	_swaybar->epollfd = epollfd;

	return 0;
}
