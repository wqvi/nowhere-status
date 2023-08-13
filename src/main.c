#include <getopt.h>
#include "nowhere_status.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	static const struct option opts[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"offline", no_argument, 0, 'o'},
		{"location", required_argument, 0, 'l'},
		{0, 0, 0, 0}
	};

	struct nowhere_config config = {
		.offline = 0,
		.location = "muc",
	};

	int c;
	int index;
	while (c = getopt_long(argc, argv, "ol:hv", opts, &index), c != -1) {
		switch (c) {
		case 'o':
			// offline
			config.offline = 1;
			break;
		case 'l':
			// location
			snprintf(config.location, 16, "%s", (char *)optarg);
			break;
		case 'h':
			// help
			printf("Usage: %s [options]\n", argv[0]);
			puts("\t-o, --offline\t\t\tdoes not retrieve weather info");
			puts("\t-l STRING, --location=STRING\tspecify where weather is pulled from");
			puts("\t-v, --version\t\t\tprints version and exits");
			puts("\t-h, --help\t\t\tprints this message and exits");
			return EXIT_SUCCESS;
		case 'v':
			// version
			puts("nowhere-status v0.1 2023 mynahisnowhere and contributors");
			return EXIT_SUCCESS;
		default:
			return EXIT_FAILURE;
		}
	}

	struct nowhere_swaybar swaybar;
	if (swaybar_create(&swaybar, &config) == -1) {
		return EXIT_FAILURE;
	}

	if (swaybar_start(&swaybar) == -1) {
		swaybar_destroy(&swaybar);
		return EXIT_FAILURE;
	}

	swaybar_destroy(&swaybar);

	return EXIT_SUCCESS; 
}
