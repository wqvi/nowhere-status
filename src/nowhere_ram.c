#include "nowhere_read.h"

static int nowhere_line_cmp(const char *_haystack, const char *_needle) {
	return (strncmp(_haystack, _needle, strlen(_needle)) == 0);
}

int nowhere_ram() {
	long double total = 0;
	long double free = 0;
	long double buffers = 0;
	long double cached = 0;

	char buffer[4096];
	if (nowhere_read(buffer, 4096, "/proc/meminfo")) return -1;

	char *line = strtok(buffer, "\n");
	do {
		if (nowhere_line_cmp(line, "MemTotal:")) {
			total = strtold(line + strlen("MemTotal:"), NULL);
		} else if (nowhere_line_cmp(line, "MemFree:")) {
			free = strtold(line + strlen("MemFree:"), NULL);
		} else if (nowhere_line_cmp(line, "Buffers:")) {
			buffers = strtold(line + strlen("Buffers:"), NULL);
		} else if (nowhere_line_cmp(line, "Cached:")) {
			cached = strtold(line + strlen("Cached:"), NULL);
		}

		// early exit
		if (!total && !free && !buffers && !cached) break;
	} while ((line = strtok(NULL, "\n")));

	// I hope your system is little endian
	long double gb = 1 << 20;
}
