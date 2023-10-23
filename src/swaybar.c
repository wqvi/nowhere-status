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
		return 1;
	}
	
	epollfd = epoll_create1(EPOLL_CLOEXEC);
	if (epollfd == -1) {
		close(timerfd);
		return 1;
	}

	if (add_events(timerfd, epollfd)) {
		close(timerfd);
		close(epollfd);
		return 1;
	}
		
	_swaybar->timerfd = timerfd;
	_swaybar->epollfd = epollfd;

	return 0;
}

int swaybar_create(struct nowhere_swaybar **_swaybar) {
	struct nowhere_swaybar *swaybar = malloc(sizeof(struct nowhere_swaybar));
	if (!swaybar) {
		return 1;
	}

	// The integral part of the program
	// if one of these parts that need to be initialized fail
	// the application WILL not run
	
	if (swaybar_fd(swaybar)) {
		free(swaybar);
		return 1;
	}

	struct node_info infos[6] = {
		{
			.flags = NOWHERE_NODE_DEFAULT,
			.name = "player",
			.fun = nowhere_player
		},
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

	// battery, date, network, ram, temperature
	if (llist_create(&swaybar->head, infos, 6) == -1) {
		free(swaybar);
		return 1;
	}

	*_swaybar = swaybar;
	
	return 0;
}

static int find_node_name(char *_buffer, char *_name) {
	char *line = strtok(_buffer, "\n");
	char *end;
	uintptr_t diff;
	do {
		char *str = strstr(line, "name");
		char *start;
		if (str == NULL) {
			continue;
		}
		
		str += strlen("name\": \"");
		start = str;
		end = strstr(start, "\"");
		if (end == NULL) {
			continue;
		}
		
		diff = end - start;
		snprintf(_name, 16, "%s", start);
		// null terminate at point where name ends
		_name[diff] = '\0';

		return 1;
	} while ((line = strtok(NULL, "\n")));

	return 0;
}

static int swaybar_poll(struct nowhere_swaybar *_swaybar, struct epoll_event *_events) {
	int avail = epoll_wait(_swaybar->epollfd, _events, 2, -1);
	for (int i = 0; i < avail; i++) {
		struct epoll_event *event = &_events[i];

		if (event->data.fd == STDIN_FILENO) { // received click event
			char buffer[BUFSIZ];
			char name[16] = { 0 };
			if (read(STDIN_FILENO, buffer, BUFSIZ) < 0) {
				return 1;
			}

			if (!find_node_name(buffer, name)) {
				continue;
			}

			struct node *node = llist_get(_swaybar->head, name);
			if (!node) {
				continue;
			}
			
			if (node->alt_text[0] == '\0') {
				continue;
			}
			
			node->flags ^= NOWHERE_NODE_ALT;
		} else if (event->data.fd == _swaybar->timerfd) { // update standard nodes timer event
			uint64_t exp;
			if (read(_swaybar->timerfd, &exp, sizeof(uint64_t)) < 0) {
				return 1;
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
		
		if (swaybar_poll(_swaybar, events)) {
			return 1;
		}
				
		llist_print(_swaybar->head);
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
