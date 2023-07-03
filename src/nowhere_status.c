#include "nowhere_status.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

static volatile sig_atomic_t nowhere_terminate = 0;

static int nowhere_parse_args(int argc, char **argv) {
	static const struct option opts[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"offline", no_argument, 0, 'o'},
		{"ifname", no_argument, 0, 'i'},
		{"thermal_zone", no_argument, 0, 't'},
		{0, 0, 0, 0}
	};

	int c;
	int index;
	while (c = getopt_long(argc, argv, "t:i:hv", opts, &index), c != -1) {
		switch (c) {
		case 'o':
			// offline
			break;
		case 't':
			// thermal zone
			break;
		case 'i':
			// ifname
			break;
		case 'h':
			// help
			printf("Usage: %s [options]\n", argv[0]);
			puts("\t--offline\t\t\tdoes not retrieve weather info");
			puts("\t-t NUMBER, --thermal_zone=NUMBER");
			puts("\t\t\t\t\tspecify cpu temperature zone");
			puts("\t-i STRING, --ifname=STRING\tspecify wireless device");
			puts("\t-v, --version\t\t\tprints version and exits");
			puts("\t-h, --help\t\t\tprints this message and exits");
			return 1;
		case 'v':
			// version
			puts("nowhere-status v0.1 2023 mynahisnowhere and contributors");
			return 1;
		default:
			return -1;
		}
	}

	return 0;
}

static void nowhere_blit(char *blk1, char *blk2) {
	printf(",[%s%s]\n", blk1, blk2);
	fflush(stdout);
}

int main(int argc, char **argv) {
	int code = nowhere_parse_args(argc, argv);
	if (code != 0)
		return code;


	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);

	struct itimerspec timerspec = {
		.it_interval = {
			.tv_sec = 5,
			.tv_nsec = 0 
		},
		.it_value = {
			.tv_sec = now.tv_sec,
			.tv_nsec = 0
		}
	};
	
	int timerfd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC);

	timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &timerspec, NULL);

	int epollfd = epoll_create1(EPOLL_CLOEXEC);
	
	struct epoll_event event = {
		.events = EPOLLIN,
		.data = {
			.fd = timerfd
		}
	};

	struct epoll_event stdin_event = {
		.events = EPOLLIN,
		.data = {
			.fd = STDIN_FILENO
		}
	};

	struct epoll_event events[2];

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &event) != 0) {
		perror("epoll_ctl");
		return -1;
	}

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event) != 0) {
		perror("epoll_ctl");
		return -1;
	}

	uint64_t exp = 1;
	printf("{\"version\":1,\"click_events\":true}\n");
	printf("[\n");
	printf("[]");
	fflush(stdout);
	fflush(stdin);
	char blk1[512] = "{\"full_text\":\"blk1\"},";
	char blk2[512] = "{\"full_text\":\"blk2\"},";
	for (;;) {
		int num = epoll_wait(epollfd, events, 2, -1);
		for (int i = 0; i < num; i++) {
			struct epoll_event *e = &events[i];
			if (e->data.fd == timerfd) {
				size_t size = read(timerfd, &exp, sizeof(uint64_t));
				if (size != sizeof(uint64_t)) {
					close(epollfd);
					close(timerfd);
					return -1;
				}
				snprintf(blk1, 512, "{\"full_text\":\"timerfd %ld\"},", exp);
			} else if (e->data.fd == STDIN_FILENO) {
				char buffer[128];

				size_t size = read(STDIN_FILENO, buffer, 128);
				snprintf(blk2, 512, "{\"color\":\"#FF0000\",\"full_text\":\"blk2\"},");
			}
		}
		nowhere_blit(blk1, blk2);
		snprintf(blk2, 512, "{\"full_text\":\"blk2\"},");
	}

	close(epollfd);
	close(timerfd);

	/*while (!nowhere_terminate) {
		printf("[");
		
		struct nowhere_network_info net = {
			.ifname = "wlan0"
		};
		nowhere_network(&net);
		nowhere_ram();
		nowhere_temperature(0);
		struct nowhere_battery_info bat;
		nowhere_battery(&bat);
		nowhere_date();

		printf("],\r");
		fflush(stdout);

		// rudimentary way of updating every minute
		// it should update based on the block status
		sleep(60);
	}*/

	return 0;
}
