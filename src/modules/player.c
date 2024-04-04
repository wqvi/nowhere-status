#include "nowhere_status.h"
#include "playerctl/playerctl.h"
#include <stdio.h>

struct player_info {
	char title[16];
	char artist[16];
};

static void tidy(char *_str, size_t _initial_length) {
	for (int i = 15; i >= 0; i--) {
		if (_str[i] == ' ' || _str[i] == '\0') {
			_str[i] = '\0';
		} else {
			break;
		}
	}

	// appends an ellipsis to string
	if (_str[15] == '\0' && _initial_length < 15) {
		return;
		
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
}

static int get_info(PlayerctlPlayer *_player, struct player_info *_info) {
	gchar *title = playerctl_player_get_title(_player, NULL);
	gchar *artist = playerctl_player_get_artist(_player, NULL);
	
	if (title == NULL) {
		return 1;
	}

	// snprintf null terminates these strings
	// that is undesirable for the tidy function
	memcpy(_info->title, title, 16);
	size_t len = strlen(title);
	tidy(_info->title, len);
	memcpy(_info->artist, artist, 16);
	len = strlen(artist);
	tidy(_info->artist, len);

	return 0;
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
	if (get_info(player, &info)) {
		// when nothing is playing
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "Not playing");
		return 0;
	}

	snprintf(_node->full_text, NOWHERE_TXTSIZ, "%.16s", info.title);
	if (info.artist[0] != '\0') {
		snprintf(_node->full_text, NOWHERE_TXTSIZ, "%.16s by %.16s", info.title, info.artist);
	}

	return 0;
}
