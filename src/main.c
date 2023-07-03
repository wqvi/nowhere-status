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
		{"ifname", required_argument, 0, 'i'},
		{"thermal_zone", required_argument, 0, 't'},
		{"location", required_argument, 0, 'l'},
		{0, 0, 0, 0}
	};

	struct nowhere_config config = {
		.offline = 0,
		.zone = 0,
		.ifname = "wlan0",
		.location = "muc",
	};

	int c;
	int index;
	while (c = getopt_long(argc, argv, "t:i:l:hv", opts, &index), c != -1) {
		switch (c) {
		case 'o':
			// offline
			config.offline = 1;
			break;
		case 't':
			// thermal zone
			config.zone = (int)strtol(optarg, NULL, 10);
			if (config.zone < 0) {
				fprintf(stderr, "Thermal zone can't be a negative number\n");
				return EXIT_FAILURE;
			}
			printf("config.zone = %d\n", config.zone);
			break;
		case 'i':
			// ifname
			strcpy(config.ifname, optarg);
			break;
		case 'l':
			// location
			strcpy(config.location, optarg);
			break;
		case 'h':
			// help
			printf("Usage: %s [options]\n", argv[0]);
			puts("\t-o, --offline\t\t\tdoes not retrieve weather info");
			puts("\t-l, --location=STRING\t\tspecify where weather is pulled from");
			puts("\t-t NUMBER, --thermal_zone=NUMBER");
			puts("\t\t\t\t\tspecify cpu temperature zone");
			puts("\t-i STRING, --ifname=STRING\tspecify wireless device");
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
	if (nowhere_swaybar_create(&swaybar, &config) == -1) return EXIT_FAILURE;
	
	if (nowhere_swaybar_start(&swaybar) == -1) {
		nowhere_swaybar_destroy(&swaybar);
		return EXIT_FAILURE;
	}

	nowhere_swaybar_destroy(&swaybar);

	return EXIT_SUCCESS; 
}
