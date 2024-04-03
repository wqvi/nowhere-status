#include "nowhere_status.h"
#include "playerctl/playerctl.h"
#include <stdio.h>

struct player_info {
	char title[16];
	char artist[16];
};

static void tidy(char *_str) {
	for (int i = 15; i >= 0; i--) {
		if (_str[i] == ' ' || _str[i] == '\0') {
			_str[i] = '\0';
		} else {
			break;
		}
	}

	size_t length = strlen(_str);
	if (length >= 15) {
		_str[15] = '.';
		_str[14] = '.';
		_str[13] = '.';
	}
}

static int get_info(PlayerctlPlayer *_player, struct player_info *_info) {
	gchar *title = playerctl_player_get_title(_player, NULL);
	gchar *artist = playerctl_player_get_artist(_player, NULL);
	
	if (title == NULL || artist == NULL) {
		return 1;
	}

	snprintf(_info->title, 16, "%s", title);
	tidy(_info->title);
	snprintf(_info->artist, 16, "%s", artist);
	tidy(_info->artist);

	return 0;
}

int nowhere_player(struct node *_node) {
	snprintf(_node->name, NOWHERE_NAMSIZ, "player");
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

	snprintf(_node->full_text, NOWHERE_TXTSIZ, "%.16s by %.16s", info.title, info.artist);

	return 0;
}
