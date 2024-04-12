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
void sstrl(char *_str, size_t _len) {
#else
static void sstrl(char *_str, size_t _len) {
#endif
	char b = _str[_len];
	for (int i = _len; i > 0; i--) {
		char c = _str[i - 1];
		_str[i - 1] = b;
		b = c;
	}
}

#ifdef DEBUG
void trim_whitespace(char *_str, size_t _len) {
#else
static void trim_whitespace(char *_str, size_t _len) {
#endif
	for (int i = _len; i >= 0; i--) {
		if (_str[i] == ' ' || _str[i] == '\0' || _str[i] == '(') {
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
int sanitize(char *_str, const char *_initial_str, size_t _initial_length) {
#else
static int sanitize(char *_str, const char *_initial_str, size_t _initial_length) {
#endif
	// title is from the website known as 'X'
	if (_initial_str[_initial_length - 1] == 'X') {
		return 'X';
	}

	// title is too long to display.
	if (_initial_length > 64) {
		return 'L';
	}

	int len = 16;
	memset(_str, 0, 16);
	if (_initial_length < 16) {
		len = strlen(_initial_str) + 1;
		memcpy(_str, _initial_str, len);
	} else {
		memcpy(_str, _initial_str, 16);
	}

	trim_whitespace(_str, len);

	sanitize_double_quotes(_str, len);

	int b = 0;
	for (int i = len - 1; i < _initial_length; i++) {
		if (_initial_str[i] == '(') {
			b = 1;	
		}
	}

	if (b) {
		return 0;
	}

	// appends an ellipsis to string
	for (int i = len; i >= len / 2; i--) {
		if (_str[i] == ' ' || _str[i] == '&' || _str[i] == '\0') {
			continue;
		}

		int j;
		for (j = 0; j < 4; j++) {
			if (_str[i - j] == ' ' || _str[i - j] == '&' || _str[i - j] == '\0') {
				break;
				j = 0;
			}
		}

		if (j >= 4) {
			_str[i] = '.';
			_str[i - 1] = '.';
			_str[i - 2] = '.';
			memset(_str + i + 1, 0, len - i);
			break;
		}
	}

	return 0;
}

static int get_info(PlayerctlPlayer *_player, struct player_info *_info) {
	gchar *title = playerctl_player_get_title(_player, NULL);
	gchar *artist = playerctl_player_get_artist(_player, NULL);
	
	if (title == NULL) {
		return 1;
	}

	// snprintf null terminates these strings
	// that is undesirable for the sanitize function
	size_t len = strlen(title);
	int ret = sanitize(_info->title, title, len);
	len = strlen(artist);
	sanitize(_info->artist, artist, len);

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
