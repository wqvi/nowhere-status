#ifndef NOWHERE_STATUS_H
#define NOWHERE_STATUS_H

#include "nowhere_swaybar.h"

int nowhere_battery(struct nowhere_node *_node);

int nowhere_date(struct nowhere_node *_node);

int nowhere_network(struct nowhere_node *_node, const char *_ifname);

int nowhere_ram(struct nowhere_node *_node);

int nowhere_temperature(struct nowhere_node *_node, int _zone);

int nowhere_weather(struct nowhere_node *_node);

#endif // NOWHERE_STATUS_H
