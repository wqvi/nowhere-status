#include "nowhere_status.h"
#include "playerctl/playerctl.h"
#include <stdio.h>

struct player_info {
	char title[16];
	char artist[16];
};

#ifdef DEBUG
void sstrr(char *_str, size_t _len) {
#else
static void sstrr(char *_str, size_t _len) {
#endif
	char b = _str[0];
	for (int i = 0; i < _len - 1; i++) {
		char c = _str[i + 1];
		_str[i + 1] = b;
		b = c;
	}
}

#ifdef DEBUG
void trim_whitespace(char *_str, size_t _len) {
#else
static void trim_whitespace(char *_str, size_t _len) {
#endif
	for (int i = _len; i >= 0; i--) {
		if (_str[i] == ' ' || _str[i] == '\0' || _str[i] == '(' || _str[i - 1] == '(') {
			_str[i] = '\0';
		} else {
			break;
		}
	}
}

#ifdef DEBUG
void sanitize_double_quotes(char *_str, size_t _len) {
#else
static void sanitize_double_quotes(char *_str, size_t _len) {
#endif
	for (int i = 0; i < _len; i++) {
		if (_str[i] == '\"') {
			sstrr(_str + i, _len - i);
			_str[i] = '\\';
			i++;
		}
	}
}

#ifdef DEBUG
int sanitize(char *_str, const char *_initial_str) {
#else
static int sanitize(char *_str, const char *_initial_str) {
#endif
	size_t initial_length = strlen(_initial_str);
	char buffer[64];
	memset(buffer, 0, sizeof(buffer));
	memset(_str, 0, 16);

	// title is from the website known as 'X'
	if (_initial_str[initial_length - 1] == 'X') {
		return 'X';
	}

	// title is too long to display.
	if (initial_length > 64) {
		//return 'L';
	}

	if (initial_length < 15) {
		memcpy(_str, _initial_str, initial_length);

		trim_whitespace(_str, initial_length);

		sanitize_double_quotes(_str, initial_length);

		return 0;
	}

	memcpy(buffer, _initial_str, initial_length);

	trim_whitespace(buffer, 16);

	sanitize_double_quotes(buffer, initial_length);

	/*for (int i = initial_length; i >= 0; i--) {
		if (buffer[i] != ' ' || buffer[i] == '\0') {
			continue;
		}
		i--;

		int j;
		int k = 0;
		for (j = 0; j < 6; j++) {
			if (buffer[i - j] == ' ' && j > 4) {
				k = 1;
				break;
			}
		}

		if (k) {
			buffer[i + 1] = '\0';
			buffer[i] = '.';
			buffer[i - 1] = '.';
			buffer[i - 2] = '.';
			break;
		}
	}*/

	memcpy(_str, buffer, 16);

	return 0;
}

static int get_info(PlayerctlPlayer *_player, struct player_info *_info) {
	gchar *title = playerctl_player_get_title(_player, NULL);
	gchar *artist = playerctl_player_get_artist(_player, NULL);
	
	if (title == NULL) {
		return 1;
	}

	int ret = sanitize(_info->title, title);
	sanitize(_info->artist, artist);

	return ret;
}

int nowhere_player(struct node *_node) {
	_node->name = 'p';
	PlayerctlPlayer *player = playerctl_player_new(NULL, NULL);
	if (player == NULL) {
		// usually only present when first booted
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "Not playing");
		return 0;
	}

	struct player_info info;
	int ret = get_info(player, &info);
	switch (ret) {
	case 1:
		// when nothing is playing
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "No media playing");
		return 0;
	case 'X':
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "Website 'X' is playing media");
		return 0;
	case 'L':
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "Media playing");
		return 0;
	}

	snprintf(_node->full_text, NOWHERE_TXTSIZ, "%.16s", info.title);
	if (info.artist[0] != '\0') {
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "%.16s by %.16s", info.title, info.artist);
	}

	return 0;
}
