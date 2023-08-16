#include "nowhere_status.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

static int get_timespec(struct itimerspec *_timerspec) {
	struct timespec now;
	if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
		return 1;
	}

	_timerspec->it_interval.tv_sec = 60;
	_timerspec->it_value.tv_sec = now.tv_sec;

	return 0;
}

static int create_timerfd(int *_timerfd) {
	struct itimerspec timerspec = { 0 };
	int timerfd;

	if (get_timespec(&timerspec)) {
		return 1;
	}
	
	timerfd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK);
	if (timerfd == -1) {
		return 1;
	}

	if (timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &timerspec, NULL) == -1) {
		close(timerfd);
		return 1;
	}

	*_timerfd = timerfd;

	return 0;
}

static int add_events(int _timerfd, int _epollfd) {
	struct epoll_event stdin_event = {
		.events = EPOLLIN,
		.data = {
			.fd = STDIN_FILENO
		}
	};
	struct epoll_event timer_event = {
		.events = EPOLLIN | EPOLLET,
		.data = {
			.fd = _timerfd
		}
	};

	// I found if STDIN_FILENO is added first then STDIN will actually
	// be polled. If STDIN is after the other FDs it'll not recieve input
	// properly
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event) < 0) {
		return 1;
	}

	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, _timerfd, &timer_event) < 0) {
		return 1;
	}

	return 0;
}

static int swaybar_fd(struct nowhere_swaybar *_swaybar) {
	int timerfd;
	int epollfd;

	if (create_timerfd(&timerfd)) {
		return -1;
	}
	
	epollfd = epoll_create1(EPOLL_CLOEXEC);
	if (epollfd == -1) {
		close(timerfd);
		return -1;
	}

	if (add_events(timerfd, epollfd)) {
		close(timerfd);
		close(epollfd);
		return -1;
	}
		
	_swaybar->timerfd = timerfd;
	_swaybar->epollfd = epollfd;

	return 0;
}

int swaybar_create(struct nowhere_swaybar **_swaybar, struct nowhere_config *_config) {
	struct nowhere_swaybar *swaybar = malloc(sizeof(struct nowhere_swaybar));
	if (!swaybar) {
		return -1;
	}

	// The integral part of the program
	// if one of these parts that need to be initialized fail
	// the application WILL not run
	
	if (swaybar_fd(swaybar) == -1) {
		free(swaybar);
	}

	struct node_info infos[5] = {
		{ 
			.flags = NOWHERE_NODE_DEFAULT | NOWHERE_NODE_ALT, 
			.name = "wireless",
			.fun = nowhere_network
		},
		{ 
			.flags = NOWHERE_NODE_DEFAULT,
			.name = "ram",
			.fun = nowhere_ram
		},
		{
			.flags = NOWHERE_NODE_DEFAULT | NOWHERE_NODE_COLOR,
			.name = "temp",
			.fun = nowhere_temperature
		},
		{ 
			.flags = NOWHERE_NODE_DEFAULT | NOWHERE_NODE_COLOR, 
			.name = "bat",
			.fun = nowhere_battery
		},
		{ 
			.flags = NOWHERE_NODE_DEFAULT | NOWHERE_NODE_ALT, 
			.name = "date",
			.fun = nowhere_date
		}
	};

	// battery, date, network, ram, temperature, weather
	if (nowhere_map_create(&swaybar->head, infos, 5) == -1) {
		free(swaybar);
	}

	memcpy(&swaybar->config, _config, sizeof(struct nowhere_config));

	*_swaybar = swaybar;
	
	return 0;
}

static void nowhere_find_node_name(char *_buffer, char *_name) {
	char *line = strtok(_buffer, "\n");
	do {
		// I personally am not a big fan of json parsing
		// so in order to avoid using a library I do a little
		// trickery here and just hash the name that is retrieved
		// It's mildly hardcoded here but it seems the swaybar json
		// protocol seems to be the same everywhere.
		char *str = strstr(line, "name");
		char *start;
		if (str != NULL) {
			str += strlen("name");
			str += strlen("\": \"");
			start = str;
			char *end = strstr(start, "\"");
			if (end != NULL) {
				snprintf(_name, 16, "%s", start);
				uintptr_t diff = end - start;
				// null terminate early because hashing the name with
				// trailing bits will not yield good results
				_name[diff] = 0;
			}
		}
	} while ((line = strtok(NULL, "\n")));
}

static int swaybar_poll(struct nowhere_swaybar *_swaybar, struct epoll_event *_events) {
	int avail = epoll_wait(_swaybar->epollfd, _events, 2, -1);
	for (int i = 0; i < avail; i++) {
		struct epoll_event *event = &_events[i];

		if (event->data.fd == STDIN_FILENO) { // received click event
			char buffer[BUFSIZ];
			char name[16] = { 0 };
			if (read(STDIN_FILENO, buffer, BUFSIZ) < 0) {
				return -1;
			}

			nowhere_find_node_name(buffer, name);
			if (name[0] != '\0') {
				struct node *node = nowhere_map_get(_swaybar->head, name);
				if (node) {
					if (node->alt_text[0] != '\0') node->flags ^= NOWHERE_NODE_ALT;
				}
			}
		} else if (event->data.fd == _swaybar->timerfd) { // update standard nodes timer event
			uint64_t exp;
			if (read(_swaybar->timerfd, &exp, sizeof(uint64_t)) < 0) {
				return -1;
			}
		}
	}

	return 0;
}

int swaybar_start(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) {
		return 1;
	}

	struct epoll_event events[3];
	puts("{\"version\":1,\"click_events\":true}\n[[]");
	for (;;) {
		struct node *head = _swaybar->head;
		while (head != NULL) {
			head->fun(head);
			head = head->next;
		}
		
		swaybar_poll(_swaybar, events);
				
		nowhere_map_print(_swaybar->head);
	}

	return 0;
}

void swaybar_destroy(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) {
		return;
	}

	close(_swaybar->epollfd);
	close(_swaybar->timerfd);
	free(_swaybar->head);
}
