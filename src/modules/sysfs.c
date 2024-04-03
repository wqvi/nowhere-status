#include <fcntl.h>
#include <math.h>
#include "nowhere_status.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef BAT0
#define BAT0 "/sys/class/power_supply/BAT0/"
#endif

#ifndef DIS
#define DIS "Discharging\n"
#endif

#ifndef CHR
#define CHR "Charging\n"
#endif

#ifndef FUL
#define FUL "Full\n"
#endif

#ifndef NOT_CHR
#define NOT_CHR "Not charging\n"
#endif

#define SYSBUFSIZ 4096

static int sysread(char *_buffer, const char *_file) {
	int fd = open(_file, O_RDONLY);
	if (fd == -1) {
		return 1;
	}

	struct stat stat;
	if (fstat(fd, &stat) == -1) {
		goto error;
	}

	if (stat.st_size != SYSBUFSIZ) {
		goto error;
	}
	
	if (read(fd, _buffer, stat.st_size) == 0) {
		goto error;
	}

	close(fd);
	return 0;

error:
	close(fd);
	return 1;
}

int nowhere_battery(struct node *_node) {
	char buffer[SYSBUFSIZ];

	if (sysread(buffer, BAT0"capacity")) {
		return -1;
	}
	int capacity = (int)strtol(buffer, NULL, 0);
	
	if (sysread(buffer, BAT0"status")) {
		return -1;
	}

	char status[4] = "UNK";

	// Assuming implementation of /sys/class/power_supply/BAT0/status
	// prints "Discharging", "Charging", "Full", and "Not charging"
	switch (buffer[0]) {
	case 'D':
		snprintf(status, 4, "DIS");
		break;
	case 'C':
		snprintf(status, 4, "CHR");
		break;
	case 'F':
		snprintf(status, 4, "FUL");
		break;
	case 'N':
		snprintf(status, 4, "INH");
		break;
	}

	float normal = 1.0 - log(1.0 + ((float)capacity / 100.0f));

	int red = normal * 255.0f;
	int green = (1.0f - normal) * 255.0f;

	snprintf(_node->name, NOWHERE_NAMSIZ, "bat");
	snprintf(_node->full_text, NOWHERE_TXTSIZ, "BAT0 %d%% %s", capacity, status);
	_node->alt_text[0] = '\0';
	_node->color.r = red;
	_node->color.g = green;
	_node->color.b = 0;

	return 0;
}

int nowhere_temperature(struct node *_node) {
	char buffer[4096];
	
	if (sysread(buffer, "/sys/class/thermal/thermal_zone0/temp")) {
		return -1;
	}

	int temp = (int)(strtod(buffer, NULL) / 1000.0);

	float normal = 1.0 - log(1.0 + (temp / 100.0f));

	int red = (1.0f - normal) * 255.0f;
	int green = normal * 255.0f;

	snprintf(_node->name, NOWHERE_NAMSIZ, "temp");
	snprintf(_node->full_text, NOWHERE_TXTSIZ, "%d\U000000B0C", temp);
	_node->alt_text[0] = '\0';
	_node->color.r = red;
	_node->color.g = green;
	_node->color.b = 0;
	_node->color._unused = 1;

	return 0;
}
