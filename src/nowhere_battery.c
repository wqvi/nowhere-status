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

int nowhere_battery(struct nowhere_bat_info *_bat) {
	char buffer[4096];

	if (nowhere_read(buffer, 4096, BAT0"capacity") == -1) return -1;
	_bat->capacity = (int)strtol(buffer, NULL, 0);
	
	if (nowhere_read(buffer, 4096, BAT0"status") == -1) return -1;
	
	if (strncmp(buffer, DIS, sizeof(DIS)) == 0) {
		_bat->status = 0;
	} else if (strncmp(buffer, CHR, sizeof(CHR)) == 0) {
		_bat->status = 1;
	} else if (strncmp(buffer, FUL, sizeof(FUL)) == 0) {
		_bat->status = 2;
	} else if (strncmp(buffer, NOT_CHR, sizeof(NOT_CHR)) == 0) {
		_bat->status = 3;
	} else {
		return -1;
	}

	float normal = 1.0 - log(1.0 + ((float)_bat->capacity / 100.0f));

	int red = normal * 255.0f;
	int green = (1.0f - normal) * 255.0f;

	printf("{\"name\":\"bat0\",\"color\":\"#%02x%02x00\",\"full_text\":\"BAT0 %d%% %s\"},", red, green, _bat->capacity, "UNK");

	return 0;
}
