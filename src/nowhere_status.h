#ifndef NOWHERE_STATUS_H
#define NOWHERE_STATUS_H

int nowhere_battery();

int nowhere_date();

int nowhere_network(const char *_ifname);

int nowhere_ram();

int nowhere_temperature(int _zone);

#endif // NOWHERE_STATUS_H
