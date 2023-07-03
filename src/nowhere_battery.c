#include "nowhere_status.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "nowhere_read.h"

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

int nowhere_battery(struct nowhere_node *_node) {
	char buffer[4096];

	if (nowhere_device_read(buffer, 4096, BAT0"capacity") == -1) return -1;
	int capacity = (int)strtol(buffer, NULL, 0);
	
	if (nowhere_device_read(buffer, 4096, BAT0"status") == -1) return -1;

	char status[4] = "UNK";
	if (strncmp(buffer, DIS, sizeof(DIS)) == 0) {
		snprintf(status, 4, "DIS");
	} else if (strncmp(buffer, CHR, sizeof(CHR)) == 0) {
		snprintf(status, 4, "CHR");
	} else if (strncmp(buffer, FUL, sizeof(FUL)) == 0) {
		snprintf(status, 4, "FUL");
	} else if (strncmp(buffer, NOT_CHR, sizeof(NOT_CHR)) == 0) {
		snprintf(status, 4, "INH");
	}

	float normal = 1.0 - log(1.0 + ((float)capacity / 100.0f));

	int red = normal * 255.0f;
	int green = (1.0f - normal) * 255.0f;

	snprintf(_node->name, NOWHERE_NAMSIZ, "bat");
	snprintf(_node->full_text, NOWHERE_TXTSIZ, "BAT0 %d%% %s", capacity, status);
	snprintf(_node->alt_text, NOWHERE_NAMSIZ, "battery alt");
	_node->color.r = red;
	_node->color.g = green;
	_node->color.b = 0;
	_node->color._unused = 1;

	return 0;
}
