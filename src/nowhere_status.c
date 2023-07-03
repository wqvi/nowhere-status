#include "nowhere_status.h"

#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/ioctl.h>
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

int main(int argc, char **argv) {
	int code = nowhere_parse_args(argc, argv);
	if (code != 0)
		return code;


	struct nowhere_swaybar swaybar;
	if (nowhere_swaybar_create(&swaybar) == -1) return EXIT_FAILURE;
	
	if (nowhere_swaybar_poll(&swaybar) == -1) {
		nowhere_swaybar_destroy(&swaybar);
		return EXIT_FAILURE;
	}

	nowhere_swaybar_destroy(&swaybar);

	return EXIT_SUCCESS; 
}
