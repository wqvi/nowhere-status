#include <math.h>
#include "nowhere_status.h"
#include <stdio.h>
#include <stdlib.h>

int nowhere_temperature(struct node *_node) {
	char buffer[4096];
	
	if (nowhere_device_read(buffer, 4096, "/sys/class/thermal/thermal_zone0/temp") == -1) return -1;

	int temp = (int)(strtod(buffer, NULL) / 1000.0);

	float normal = 1.0 - log(1.0 + (temp / 100.0f));

	int red = (1.0f - normal) * 255.0f;
	int green = normal * 255.0f;

	_node->flags = NOWHERE_NODE_DEFAULT | NOWHERE_NODE_COLOR;
	snprintf(_node->name, NOWHERE_NAMSIZ, "temp");
	snprintf(_node->full_text, NOWHERE_TXTSIZ, "%d\U000000B0C", temp);
	_node->alt_text[0] = '\0';
	_node->color.r = red;
	_node->color.g = green;
	_node->color.b = 0;
	_node->color._unused = 1;

	return 0;
}
