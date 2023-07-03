#include <stdio.h>
#include <time.h>

int nowhere_date() {
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	if (tm == NULL) return -1;

	// ISO Date format, 24 hours
	// YYYY-MM-DD HH:MM
	char date[24];
	strftime(date, 24, "%F %R", tm);

	printf("{\"name\":\"date\",\"full_text\":\"%s\"},", date);

	return 0;
}
