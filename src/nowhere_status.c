#include "nowhere_status.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t nowhere_terminate = 0;

static int nowhere_parse_args(int argc, char **argv) {
	static const struct option opts[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"offline", no_argument, 0, 0},
		{0, 0, 0, 0}
	};

	int c;
	int index;
	while (c = getopt_long(argc, argv, "t:i:hv", opts, &index), c != -1) {
		switch (c) {
		case 0:
			if (index == 1) {
				// offline
			}
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
			puts("\t--offline\t\tdoes not retrieve weather info");
			puts("\t-t\t\t\tspecify cpu temperature zone");
			puts("\t-i\t\t\tspecify wireless device");
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

	printf("{\"version\":1, \"click_events\":true}\n[");

	while (!nowhere_terminate) {
		printf("[");

		nowhere_network("wlan0");
		nowhere_ram();
		nowhere_temperature(0);
		nowhere_battery();
		nowhere_date();

		printf("],\r");
		fflush(stdout);

		// rudimentary way of updating every minute
		// it should update based on the block status
		sleep(60);
	}

	return 0;
}
