#include "nowhere_status.h"
#include <stdio.h>
#include <string.h>

int nowhere_weather(struct nowhere_node *_node, CURL *curl, char *_buffer) {
	// 3600 seconds, one hour
	// use timerfd
	if (curl_easy_perform(curl) != CURLE_OK) return -1;

	snprintf(_node->name, NOWHERE_NAMSIZ, "weather");
	snprintf(_node->full_text, NOWHERE_TXTSIZ, "%s", _buffer);
	_node->alt_text[0] = '\0';
	_node->color._unused = 0;

	return 0;
}
