#include "nowhere_status.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
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

int nowhere_swaybar_start(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) return -1;

	struct epoll_event events[2];
	puts("{\"version\":1,\"click_events\":true}\n[[]");
	for (;;) {
		printf(",[");
		int avail = epoll_wait(_swaybar->epollfd, events, 2, -1);
		for (int i = 0; i < avail; i++) {
			struct epoll_event *event = &events[i];
			if (event->data.fd == STDIN_FILENO) {
				char buffer[BUFSIZ];
				if (read(STDIN_FILENO, buffer, BUFSIZ) < 0) return -1;
				char *line = strtok(buffer, "\n");
				char name[16] = "UNK";
				do {
					char *str = strstr(line, "name");
					char *start;
					if (str != NULL) {
						str += strlen("name");
						str += strlen("\": \"");
						start = str;
						char *end = strstr(start, "\"");
						if (end != NULL) {
							snprintf(name, 16, "%s", start);
							uintptr_t diff = end - start;
							name[diff] = 0;
						}
					}
				} while ((line = strtok(NULL, "\n")));
				printf("{\"name\":\"stdin_debug\",\"full_text\":\"stdin debug %s\"},", name);
			} else if (event->data.fd == _swaybar->timerfd) {
				uint64_t exp;
				if (read(_swaybar->timerfd, &exp, sizeof(uint64_t)) < 0) return -1;
			}
		}

		
		struct nowhere_network_info net = {
			.ifname = "wlan0"
		};
		nowhere_network(&net);
		nowhere_ram();
		nowhere_temperature(0);
		struct nowhere_battery_info bat;
		nowhere_battery(&bat);
		nowhere_date();

		printf("]\n");
		fflush(stdout);
	}
}

void nowhere_swaybar_destroy(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) return;

	close(_swaybar->epollfd);
	close(_swaybar->timerfd);
}
