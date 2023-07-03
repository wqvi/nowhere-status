#ifndef NOWHERE_STATUS_H
#define NOWHERE_STATUS_H

struct nowhere_battery_info {
	int capacity;
	int status;
};

struct nowhere_network_info {
	char ifname[16];
	char addr[16];
};

struct nowhere_block {
	struct nowhere_block *next;
};

struct nowhere_swaybar {
	int timerfd;
	int epollfd;
	struct nowhere_block *head;
};

int nowhere_swaybar_create(struct nowhere_swaybar *_swaybar);

int nowhere_battery(struct nowhere_battery_info *_bat);

int nowhere_date();

int nowhere_network(struct nowhere_network_info *_net);

int nowhere_ram();

int nowhere_temperature(int _zone);

#endif // NOWHERE_STATUS_H
