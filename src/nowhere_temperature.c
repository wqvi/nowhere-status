#include "nowhere_read.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int nowhere_temperature(int zone) {
	char file[128];
	snprintf(file, 128, "/sys/class/thermal/thermal_zone%d/temp", zone);
	char buffer[4096];
	
	if (nowhere_read(buffer, 4096, file)) return -1;

	int temp = (int)(strtod(buffer, NULL) / 1000.0);

	float normal = 1.0 - log(1.0 + (temp / 100.0f));

	return 0;
}
