#include <math.h>
#include "nowhere_read.h"
#include "nowhere_swaybar.h"
#include <stdio.h>
#include <stdlib.h>

int nowhere_temperature(struct nowhere_node *_node, int zone) {
	char file[128];
	snprintf(file, 128, "/sys/class/thermal/thermal_zone%d/temp", zone);
	char buffer[4096];
	
	if (nowhere_device_read(buffer, 4096, file)) return -1;

	int temp = (int)(strtod(buffer, NULL) / 1000.0);

	float normal = 1.0 - log(1.0 + (temp / 100.0f));

	int red = (1.0f - normal) * 255.0f;
	int green = normal * 255.0f;

	snprintf(_node->name, 16, "temp");
	snprintf(_node->full_text, 48, "%d\U000000B0C", temp);
	_node->color.r = red;
	_node->color.g = green;
	_node->color.b = 0;
	_node->color._unused = 1;

	return 0;
}
