#ifndef NOWHERE_STATUS_H
#define NOWHERE_STATUS_H

struct nowhere_bat_info {
	int capacity;
	int status;
};

int nowhere_battery(struct nowhere_bat_info *_bat);

int nowhere_date();

int nowhere_network(const char *_ifname);

int nowhere_ram();

int nowhere_temperature(int _zone);

#endif // NOWHERE_STATUS_H
