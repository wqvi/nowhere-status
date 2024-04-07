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
int sanitize(char *_str, const char *_initial_str, size_t _initial_length) {
#else
static int sanitize(char *_str, const char *_initial_str, size_t _initial_length) {
#endif
	if (_initial_str[_initial_length - 1] == 'X') {
		return 'X';
	}

	memcpy(_str, _initial_str, 16);

	for (int i = 15; i >= 0; i--) {
		if (_str[i] == ' ' || _str[i] == '\0' || _str[i] == '(') {
			_str[i] = '\0';
		} else {
			break;
		}
	}

	// Add escape sequence to double quotes
	// this is unnecessary for single quotes
	for (int i = 0; i < 15; i++) {
		if (_str[i] == '\"') {
			sstrr(_str + i, 15 - i);
			_str[i] = '\\';
			i++;
		}
	}

	// appends an ellipsis to string
	if (_initial_length < 15) {
		return 0;
		
	}

	if (_str[15 - 3] != ' ') {
		return 0;
	}

	_str[15] = '.';
	_str[14] = '.';
	_str[13] = '.';
	
	for (int i = 12; i >= 0; i--) {
		if ((_str[i] == '&' || _str[i] == ' ') && _str[i + 1] == '.') {
			_str[i] = '.';
			_str[i + 1] = '.';
			_str[i + 2] = '.';
			_str[i + 3] = '\0';
		} else {
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
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "Not playing");
		return 0;
	case 'X':
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "Website 'X' is playing media");
		return 0;
	}

	snprintf(_node->full_text, NOWHERE_TXTSIZ, "%.16s", info.title);
	if (info.artist[0] != '\0') {
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "%.16s by %.16s", info.title, info.artist);
	}

	return 0;
}
