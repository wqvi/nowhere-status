#include "nowhere_read.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int nowhere_line_cmp(const char *_haystack, const char *_needle) {
	return (strncmp(_haystack, _needle, strlen(_needle)) == 0);
}

int nowhere_ram() {
	long double memtotal = 0;
	long double memfree = 0;
	long double buffers = 0;
	long double cached = 0;

	FILE *fp;
	if (!(fp = fopen("/proc/meminfo", "rb"))) return -1;

	char line[128];
	while (fgets(line, 128, fp)) {
		if (nowhere_line_cmp(line, "MemTotal:")) {
			memtotal = strtold(line + strlen("MemTotal:"), NULL);
		} else if (nowhere_line_cmp(line, "MemFree:")) {
			memfree = strtold(line + strlen("MemFree:"), NULL);
		} else if (nowhere_line_cmp(line, "Buffers:")) {
			buffers = strtold(line + strlen("Buffers:"), NULL);
		} else if (nowhere_line_cmp(line, "Cached:")) {
			cached = strtold(line + strlen("Cached:"), NULL);
		}

		// early exit
		if (!memtotal && !memfree && !buffers && !cached) break;
	}

	fclose(fp);

	// I hope your system is little endian
	long double gb = 1 << 20;

	printf("{\"name\":\"ram\",\"full_text\":\"%.1LfGb/%.1LfGb\"},", (memtotal - memfree - buffers - cached) / gb, memtotal / gb);

	return 0;
}
