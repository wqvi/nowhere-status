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

int nowhere_battery(struct nowhere_battery_info *_bat);

int nowhere_date();

int nowhere_network(struct nowhere_network_info *_net);

int nowhere_ram();

int nowhere_temperature(int _zone);

#endif // NOWHERE_STATUS_H
