#ifndef NOWHERE_STATUS_H
#define NOWHERE_STATUS_H

#include "swaybar.h"

int nowhere_device_read(char *_buffer, size_t _size, const char *_file);

int nowhere_player(struct node *_node);

int nowhere_battery(struct node *_node);

int nowhere_date(struct node *_node);

int nowhere_network(struct node *_node);

int nowhere_ram(struct node *_node);

int nowhere_temperature(struct node *_node);

#endif // NOWHERE_STATUS_H
