#include "nowhere_status.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

static size_t curl_callback(char *_data, size_t _size, size_t _nitems, void *_buffer) {
	if (NOWHERE_TXTSIZ > _size * _nitems) {
		strncpy(_buffer, _data, _size * _nitems);
	} else {
		strncpy(_buffer, _data, NOWHERE_TXTSIZ);
	}

	// Definitely a naive implementation but the URL output
	// will only have two _ because CURL is picky and won't take
	// format='%C %w %t' and instead wants format=%C_%w_%t
	// thus the buffer will look something like "Sunny 3km/h +17C"
	size_t pos = strcspn(_buffer, "_");
	if (pos != _size * _nitems && pos < NOWHERE_TXTSIZ) {
		((char *)_buffer)[pos] = ' ';
	}

	pos = strcspn(_buffer, "_");
	if (pos != _size * _nitems && pos < NOWHERE_TXTSIZ) {
		((char *)_buffer)[pos] = ' ';
	}

	return _size * _nitems;
}

int nowhere_swaybar_create(struct nowhere_swaybar *_swaybar, struct nowhere_config *_config) {
	if (!_swaybar || !_config) return -1;

	// The integral part of the program
	// if one of these parts that need to be initialized fail
	// the application WILL not run

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

	int weatherfd = 0;
	if (!_config->offline) {
		weatherfd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK);
		if (weatherfd == -1) return -1;

		timerspec.it_interval.tv_sec = 3600; // only update weather every hour

		if(timerfd_settime(weatherfd, TFD_TIMER_ABSTIME, &timerspec, NULL) == -1) {
			close(timerfd);
			close(weatherfd);
			return -1;
		}
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

	// I found if STDIN_FILENO is added first then STDIN will actually
	// be polled. If STDIN is after the other FDs it'll not recieve input
	// properly
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event) < 0) goto error;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &timer_event) < 0) goto error;
	
	if (!_config->offline) {
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, weatherfd, &weather_event) < 0) goto error;
	}

	// battery, date, network, ram, temperature, weather
	int amount = 6 - _config->offline;
	if (nowhere_map_create(&_swaybar->map, amount) == -1) goto error;

	if (!_config->offline) {
		if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) goto error;

		_swaybar->curl = curl_easy_init();
		if (_swaybar->curl == NULL) {
			curl_global_cleanup();
			goto error;
		}

		char wttr[64];
		snprintf(wttr, 64, "https://wttr.in/%s?format=%%C_%%w_%%t", _config->location);
		curl_easy_setopt(_swaybar->curl, CURLOPT_URL, wttr);
		curl_easy_setopt(_swaybar->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
		curl_easy_setopt(_swaybar->curl, CURLOPT_WRITEFUNCTION, curl_callback);
		_swaybar->weatherfd = weatherfd;
	} else {
		_swaybar->weatherfd = 0;
	}

	_swaybar->timerfd = timerfd;
	_swaybar->epollfd = epollfd;

	memcpy(&_swaybar->config, _config, sizeof(struct nowhere_config));
	
	return 0;
error:
	close(timerfd);
	if (!_config->offline) close(weatherfd);
	close(epollfd);
	return -1;
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

static int nowhere_swaybar_poll(struct nowhere_swaybar *_swaybar, struct nowhere_node *_cache, struct epoll_event *_events, char *_buffer) {
	int avail = epoll_wait(_swaybar->epollfd, _events, 3 - _swaybar->config.offline, -1);
	for (int i = 0; i < avail; i++) {
		struct epoll_event *event = &_events[i];
		if (event->data.fd == STDIN_FILENO) {
			char buffer[BUFSIZ];
			if (read(STDIN_FILENO, buffer, BUFSIZ) < 0) return -1;
			char name[16] = "\0";
			nowhere_find_node_name(buffer, name);
			if (name[0] != '\0') {
				struct nowhere_node *node = nowhere_map_get(_swaybar->map, name);
				// something something usage flags
				if (node) {
					if (node->alt_text[0] != '\0') node->usage = !node->usage;
				}
			}
		} else if (event->data.fd == _swaybar->timerfd) {
			uint64_t exp;
			if (read(_swaybar->timerfd, &exp, sizeof(uint64_t)) < 0) return -1;
		} else if (event->data.fd == _swaybar->weatherfd && !_swaybar->config.offline) {
			uint64_t exp;
			if (read(_swaybar->weatherfd, &exp, sizeof(uint64_t)) < 0) return -1;
			nowhere_weather(_cache, _swaybar->curl, _buffer);
			nowhere_map_put(_swaybar->map, _cache);
		}
	}

	return 0;
}

int nowhere_swaybar_start(struct nowhere_swaybar *_swaybar) {
	if (!_swaybar) return -1;

	char weather[NOWHERE_TXTSIZ] = "Weather UNK";
	if (!_swaybar->config.offline) {
		// This is a workaround for hiding the buffer unnecessarily
		// in this function stack.
		// Mainly just want to keep it out of the swaybar struct because
		// I thought it would be very out of place if the buffer was there
		curl_easy_setopt(_swaybar->curl, CURLOPT_WRITEDATA, (void*)weather);
	}

	struct nowhere_node cache;
	struct epoll_event events[3];
	puts("{\"version\":1,\"click_events\":true}\n[[]");
	for (;;) {
		nowhere_swaybar_poll(_swaybar, &cache, events, weather);
		
		nowhere_network(&cache, _swaybar->config.ifname);
		nowhere_map_put(_swaybar->map, &cache);

		nowhere_ram(&cache);
		nowhere_map_put(_swaybar->map, &cache);
		
		nowhere_temperature(&cache, _swaybar->config.zone);
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
	if (!_swaybar->config.offline) close(_swaybar->weatherfd);
	close(_swaybar->timerfd);
	free(_swaybar->map);
}
