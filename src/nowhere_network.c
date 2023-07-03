#include <linux/wireless.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static int nowhere_network_common(int _fd, struct iwreq *_rq, char *_addr) {
	// Network device is up
	if (ioctl(_fd, SIOCGIFFLAGS, _rq) < 0) return -1;

	if ((((struct ifreq *)_rq)->ifr_flags & IFF_UP) != IFF_UP) return -1;
	
	// Converts numerical ip address to string address
	_rq->u.addr.sa_family = AF_INET;
	if (ioctl(_fd, SIOCGIFADDR, _rq) < 0) return -1;

	memset(_addr, 0, INET_ADDRSTRLEN);
	struct sockaddr_in *in = (struct sockaddr_in *)&_rq->u.ap_addr;
	inet_ntop(AF_INET, &in->sin_addr, _addr, INET_ADDRSTRLEN);
	
	return 0;
}

static int nowhere_wireless(int _fd, struct iwreq *_rq) {
	char addr[INET_ADDRSTRLEN];
	if (nowhere_network_common(_fd, _rq, addr)) return -1;

	// essid variable is 16 bytes over essid max size
	char essid[48];
	struct iw_statistics stats;
	struct iw_range range;

	_rq->u.essid.pointer = essid;
	_rq->u.essid.length = IW_ESSID_MAX_SIZE + 1;
	_rq->u.essid.flags = 0;

	// get network name
	if (ioctl(_fd, SIOCGIWESSID, _rq) < 0) return -1;
	
	_rq->u.data.pointer = &stats;
	_rq->u.data.length = sizeof(stats);
	_rq->u.data.flags = 0;

	// get network stats for quality calculation
	if (ioctl(_fd, SIOCGIWSTATS, _rq) < 0) return -1;

	_rq->u.data.pointer = &range;
	_rq->u.data.length = sizeof(range);
	_rq->u.data.flags = 0;
	
	// get network range for quality calculation
	if (ioctl(_fd, SIOCGIWRANGE, _rq) < 0) return -1;

	int quality = 0;
	if (stats.qual.qual != 0) {
		quality = (stats.qual.qual * 100) / range.max_qual.qual;
	} else {
		return -1;
	}
	
	return 0;
}

static int nowhere_resolve_ifname(struct iwreq *_rq, const char *_ifname) {
	struct ifaddrs *head;
	if (getifaddrs(&head)) return -1;
	
	for (struct ifaddrs *list = head; list != NULL; list = list->ifa_next) {
		// skip loopback device
		if (!strcmp(list->ifa_name, "lo")) continue;

		// prioritize vpn display
		if (!strcmp(list->ifa_name, "wg0")) {
			strncpy(_rq->ifr_name, list->ifa_name, IFNAMSIZ);
			break;
		}

		// validate provided ifname
		if (!strcmp(list->ifa_name, _ifname)) strncpy(_rq->ifr_name, _ifname, IFNAMSIZ);
	}

	freeifaddrs(head);
	
	return 0;
}

int nowhere_network(const char *_ifname) {
	struct iwreq rq;
	if (nowhere_resolve_ifname(&rq, _ifname)) return -1;

	int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (fd == -1) return -1;

	if (ioctl(fd, SIOCGIWNAME, &rq) < 0) nowhere_wireless(fd, &rq);

	close(fd);

	return 0;
}
