#include "swaybar.h"
#include <stdio.h>
#include <time.h>

int nowhere_date(struct node *_node) {
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	if (tm == NULL) return -1;

	_node->name = 'd';
	// ISO Date format, 24 hours
	// YYYY-MM-DD HH:MM
	strftime(_node->full_text, NOWHERE_TXTSIZ, "%F %R", tm);
	// ISO Date format, 12 hours
	// YYYY-MM-DD HH:MM AM/PM
	strftime(_node->alt_text, NOWHERE_TXTSIZ, "%F %I:%M %p", tm);

	return 0;
}
