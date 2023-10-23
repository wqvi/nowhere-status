#include "nowhere_status.h"
#include "playerctl/playerctl.h"
#include <stdio.h>

int nowhere_player(struct node *_node) {
	PlayerctlPlayer *player = playerctl_player_new(NULL, NULL);

	gchar *artist = playerctl_player_get_artist(player, NULL);
	gchar *title = playerctl_player_get_title(player, NULL);

	snprintf(_node->name, NOWHERE_NAMSIZ, "player");
	snprintf(_node->full_text, NOWHERE_TXTSIZ, "%s by %s", title, artist);
}
