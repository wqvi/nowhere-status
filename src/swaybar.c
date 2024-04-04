#include "nowhere_status.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

static int get_timespec(struct itimerspec *_timerspec, time_t _sec) {
	struct timespec now;
	if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
		return 1;
	}

	_timerspec->it_interval.tv_sec = _sec;
	_timerspec->it_value.tv_sec = now.tv_sec;

	return 0;
}

static int create_timerfd(int *_timerfd, time_t _sec) {
	struct itimerspec timerspec = { 0 };
	int timerfd;

	if (get_timespec(&timerspec, _sec)) {
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

static int add_events(int _epollfd, int _fd) {
	struct epoll_event event = {
		.events = EPOLLIN | EPOLLET,
		.data = {
			.fd = _fd
		}
	};

	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, _fd, &event) < 0) {
		return 1;
	}

	return 0;
}

static int swaybar_fd(struct nowhere_swaybar *_swaybar) {
	int timerfd;
	int playerctlfd;
	int epollfd;

	if (create_timerfd(&timerfd, 60)) {
		return 1;
	}

	if (create_timerfd(&playerctlfd, 5)) {
		close(timerfd);
		return 1;
	}
	
	epollfd = epoll_create1(EPOLL_CLOEXEC);
	if (epollfd == -1) {
		close(timerfd);
		close(playerctlfd);
		return 1;
	}

	// I found if STDIN_FILENO is added first then STDIN will actually
	// be polled. If STDIN is after the other FDs it'll not recieve input
	// properly
	if (add_events(epollfd, STDIN_FILENO)) {
		goto error;
	}

	if (add_events(epollfd, timerfd)) {
		goto error;
	}

	if (add_events(epollfd, playerctlfd)) {
		goto error;
	}
		
	_swaybar->timerfd = timerfd;
	_swaybar->playerctlfd = playerctlfd;
	_swaybar->epollfd = epollfd;

	return 0;

error:
	close(timerfd);
	close(playerctlfd);
	close(epollfd);
	return 1;
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
			.name = 'p',
			.fun = nowhere_player
		},
		{ 
			.flags = NOWHERE_NODE_DEFAULT | NOWHERE_NODE_ALT, 
			.name = 'w',
			.fun = nowhere_network
		},
		{ 
			.flags = NOWHERE_NODE_DEFAULT,
			.name = 'r',
			.fun = nowhere_ram
		},
		{
			.flags = NOWHERE_NODE_DEFAULT | NOWHERE_NODE_COLOR,
			.name = 't',
			.fun = nowhere_temperature
		},
		{ 
			.flags = NOWHERE_NODE_DEFAULT | NOWHERE_NODE_COLOR, 
			.name = 'b',
			.fun = nowhere_battery
		},
		{ 
			.flags = NOWHERE_NODE_DEFAULT | NOWHERE_NODE_ALT, 
			.name = 'd',
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

static int stdin_poll(struct nowhere_swaybar *_swaybar) {
	char buffer[BUFSIZ];
	char name[16] = { 0 };
	if (read(STDIN_FILENO, buffer, BUFSIZ) < 0) {
		return 1;
	}

	if (!find_node_name(buffer, name)) {
		return 0;
	}

	struct node *node = llist_get(_swaybar->head, name[0]);
	if (!node) {
		return 0;
	}
			
	if (node->alt_text[0] == '\0') {
		return 0;
	}
			
	node->flags ^= NOWHERE_NODE_ALT;

	return 0;
}

static int event_poll(int _fd) {
	uint64_t exp;
	return read(_fd, &exp, sizeof(uint64_t)) < 0;
}

static int swaybar_poll(struct nowhere_swaybar *_swaybar, struct epoll_event *_events) {
	int avail = epoll_wait(_swaybar->epollfd, _events, 3, -1);
	for (int i = 0; i < avail; i++) {
		struct epoll_event *event = &_events[i];

		if (event->data.fd == STDIN_FILENO) { // received click event
			if (stdin_poll(_swaybar)) {
				return 1;
			}
		} else if (event->data.fd == _swaybar->timerfd) { // update standard nodes timer event
			if (event_poll(_swaybar->timerfd)) {
				return 1;
			}
			struct node *head = _swaybar->head;
			while (head != NULL) {
				if (head->name != 'p') {
					head->fun(head);
				}
				head = head->next;
			}
		} else if (event->data.fd == _swaybar->playerctlfd) { // update playerctl node timer event
			if (event_poll(_swaybar->playerctlfd)) {
				return 1;
			}
			struct node *head = llist_get(_swaybar->head, 'p');
			head->fun(head);
		}
	}

	return 0;
}

int swaybar_start(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) {
		return 1;
	}

	struct epoll_event events[4];
	puts("{\"version\":1,\"click_events\":true}\n[[]");
	for (;;) {
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
