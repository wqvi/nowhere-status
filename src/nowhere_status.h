#ifndef NOWHERE_STATUS_H
#define NOWHERE_STATUS_H

#include "swaybar.h"

int nowhere_player(struct node *_node);

int nowhere_battery(struct node *_node);

int nowhere_date(struct node *_node);

int nowhere_network(struct node *_node);

int nowhere_ram(struct node *_node);

int nowhere_temperature(struct node *_node);

void sstrr(char *_str, size_t _len);

void tidy(char *_str, size_t _initial_length);

#endif // NOWHERE_STATUS_H
