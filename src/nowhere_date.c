#include "nowhere_swaybar.h"
#include <stdio.h>
#include <time.h>

int nowhere_date(struct nowhere_node *_node) {
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	if (tm == NULL) return -1;

	snprintf(_node->name, 16, "date");
	// ISO Date format, 24 hours
	// YYYY-MM-DD HH:MM
	char date[24];
	strftime(_node->full_text, 24, "%F %R", tm);
	_node->color._unused = 0;

	return 0;
}
