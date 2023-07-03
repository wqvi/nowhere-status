#include <curl/curl.h>
#include "nowhere_alloc.h"
#include "nowhere_status.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

static size_t curl_callback(char *_data, size_t _size, size_t _nitems, void *_buffer) {
	size_t pos = strcspn(_data, "_");
	if (NOWHERE_TXTSIZ > _size * _nitems) strncpy(_buffer, _data, _size * _nitems);
	else strncpy(_buffer, _data, NOWHERE_TXTSIZ);
	if (pos != _size * _nitems) {
		if (pos < NOWHERE_TXTSIZ) ((char *)_buffer)[pos] = ' ';
	}
	return _size * _nitems;
}

int nowhere_swaybar_create(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) return -1;

	struct timespec now;
	if (clock_gettime(CLOCK_REALTIME, &now) == -1) return -1;

	struct itimerspec timerspec = {
		.it_interval = { .tv_sec = 60, .tv_nsec = 0 },
		.it_value = { .tv_sec = now.tv_sec, .tv_nsec = 0 }
	};

	int timerfd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK);
	if (timerfd == -1) return -1;

	if (timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &timerspec, NULL) == -1) {
		close(timerfd);
		return -1;
	}

	int weatherfd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK);
	if (weatherfd == -1) return -1;

	timerspec.it_interval.tv_sec = 3600; // only update weather every hour

	if(timerfd_settime(weatherfd, TFD_TIMER_ABSTIME, &timerspec, NULL) == -1) {
		close(timerfd);
		close(weatherfd);
		return -1;
	}

	int epollfd = epoll_create1(EPOLL_CLOEXEC);
	if (epollfd == -1) {
		close(timerfd);
		close(weatherfd);
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

	struct epoll_event weather_event = {
		.events = EPOLLIN | EPOLLET,
		.data = { .fd = weatherfd }
	};

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event) < 0) goto error;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &timer_event) < 0) goto error;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, weatherfd, &weather_event) < 0) goto error;
	
	if (nowhere_map_create(&_swaybar->map, 6) == -1) goto error;

	if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) goto error;

	_swaybar->curl = curl_easy_init();
	if (_swaybar->curl == NULL) {
		curl_global_cleanup();
		goto error;
	}

	char wttr[64];
	snprintf(wttr, 64, "https://wttr.in/%s?format=%%C_%%w_%%t", "muc");
	curl_easy_setopt(_swaybar->curl, CURLOPT_URL, wttr);
	curl_easy_setopt(_swaybar->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
	curl_easy_setopt(_swaybar->curl, CURLOPT_WRITEFUNCTION, curl_callback);

	_swaybar->timerfd = timerfd;
	_swaybar->weatherfd = weatherfd;
	_swaybar->epollfd = epollfd;

	return 0;
error:
	close(timerfd);
	close(weatherfd);
	close(epollfd);
	return -1;
}

int nowhere_swaybar_start(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) return -1;

	char weather[NOWHERE_TXTSIZ] = "Weather UNK";
	curl_easy_setopt(_swaybar->curl, CURLOPT_WRITEDATA, (void*)weather);

	struct nowhere_node cache;
	struct epoll_event events[3];
	puts("{\"version\":1,\"click_events\":true}\n[[]");
	for (;;) {
		int avail = epoll_wait(_swaybar->epollfd, events, 3, -1);
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
				if (strcmp(name, "UNK") != 0) {
					struct nowhere_node *node = nowhere_map_get(_swaybar->map, name);
					if (node) {
						node->usage = !node->usage;
					}
				}
			} else if (event->data.fd == _swaybar->timerfd) {
				uint64_t exp;
				if (read(_swaybar->timerfd, &exp, sizeof(uint64_t)) < 0) return -1;
			} else if (event->data.fd == _swaybar->weatherfd) {
				uint64_t exp;
				if (read(_swaybar->weatherfd, &exp, sizeof(uint64_t)) < 0) return -1;
				nowhere_weather(&cache, _swaybar->curl, weather);
				nowhere_map_put(_swaybar->map, &cache);
			}
		}
		
		nowhere_network(&cache, "wlan0");
		nowhere_map_put(_swaybar->map, &cache);

		nowhere_ram(&cache);
		nowhere_map_put(_swaybar->map, &cache);
		
		nowhere_temperature(&cache, 0);
		nowhere_map_put(_swaybar->map, &cache);
		
		nowhere_battery(&cache);
		nowhere_map_put(_swaybar->map, &cache);
		
		nowhere_date(&cache);
		nowhere_map_put(_swaybar->map, &cache);
		
		nowhere_map_print(_swaybar->map);
	}
}

void nowhere_swaybar_destroy(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) return;

	close(_swaybar->epollfd);
	close(_swaybar->timerfd);
	nowhere_free(_swaybar->map);
}
