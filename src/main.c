#include <getopt.h>
#include "nowhere_status.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void help(char *rel) {
	printf("Usage: %s [options]\n", rel);
	puts("\t-v, --version\t\t\tprints version and exits");
	puts("\t-h, --help\t\t\tprints this message and exits");
}

static void version(void) {
	puts("nowhere-status v0.1 2023 mynahisnowhere and contributors");
}

static void parse_cmdline(int argc, char **argv) {
	static const struct option opts[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};

	int c;
	int index;
	while (c = getopt_long(argc, argv, "hv", opts, &index), c != -1) {
		switch (c) {
		case 'h':
			help(argv[0]);
			exit(EXIT_SUCCESS);
		case 'v':
			version();
			exit(EXIT_SUCCESS);
		default:
			exit(EXIT_FAILURE);
		}
	}
}

int main(int argc, char **argv) {
	parse_cmdline(argc, argv);
	
	struct nowhere_swaybar *swaybar;
	if (swaybar_create(&swaybar)) {
		return EXIT_FAILURE;
	}

	if (swaybar_start(swaybar)) {
		swaybar_destroy(swaybar);
		return EXIT_FAILURE;
	}

	swaybar_destroy(swaybar);

	return EXIT_SUCCESS; 
}
