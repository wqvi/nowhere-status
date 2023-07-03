#include "nowhere_status.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

static CURL *curl;

// url to weather web service
// m specifies metric, %C is plain text weather, %t is temperature
static const char *weather_service = "https://wttr.in?m&format=%C_%t";

static time_t weather_service_check = 0;

static size_t curl_callback(char *_buffer, size_t _size, size_t _nitems, void *_instream);

static int nowhere_curl_init() {
	if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) return -1;

	curl = curl_easy_init();
	if (curl == NULL) {
		curl_global_cleanup();
		return -1;
	}

	char data[NOWHERE_TXTSIZ];
	curl_easy_setopt(curl, CURLOPT_URL, weather_service);
	curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

	return 0;
}

static void nowhere_curl_clean() {
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

size_t curl_curlcallback(char *_data, size_t _size, size_t _nitems, void *_buffer) {
	size_t pos = strcspn(_data, "_");
	if (pos != _size * _nitems) _data[pos] = ' ';
	strncpy(_buffer, _data, _size * _nitems);

	return _size * _nitems;
}

int nowhere_weather(struct nowhere_node *_node) {
	if (weather_service_check == 0) {
		weather_service_check = time(NULL);
		printf("{\"full_text\":\"abcd\"},");
		if (curl_easy_perform(curl) != CURLE_OK) return -1;
		return 0;
	}

	time_t now = time(NULL);
	// 3600 seconds, one hour
	if (difftime(now, weather_service_check) > 3600) {
		weather_service_check = time(NULL);
		printf("{\"full_text\":\"dcba\"},");
		if (curl_easy_perform(curl) != CURLE_OK) return -1;
		return 0;
	}

	return 0;
}
