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
#define DIS "Discharging"
#endif

#ifndef CHR
#define CHR "Charging"
#endif

#ifndef FUL
#define FUL "Full"
#endif

struct nowhere_bat_info {
	int capacity;
	int status;
};

int nowhere_battery(struct nowhere_bat_info *_bat) {
	char buffer[4096];

	if (nowhere_read(buffer, 4096, BAT0"capacity") == -1) return -1;
	_bat->capacity = (int)strtol(buffer, NULL, 0);
	
	if (nowhere_read(buffer, 4096, BAT0"status") == -1) return -1;
	
	if (!strncmp(buffer, DIS, sizeof(DIS))) {		
		_bat->status = 0;
	} else if (!strncmp(buffer, CHR, sizeof(CHR))) {
		_bat->status = 1;
	} else if (!strncmp(buffer, FUL, sizeof(FUL))) {
		_bat->status = 2;
	} else {
		return -1;
	}

	return 0;
}
