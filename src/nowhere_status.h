#ifndef NOWHERE_STATUS_H
#define NOWHERE_STATUS_H

#include "nowhere_swaybar.h"

int nowhere_battery(struct nowhere_node *_node);

int nowhere_date();

int nowhere_network(struct nowhere_node *_node, const char *_ifname);

int nowhere_ram();

int nowhere_temperature(int _zone);

#endif // NOWHERE_STATUS_H
