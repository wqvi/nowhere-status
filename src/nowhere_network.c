#include "nowhere_status.h"

#include <linux/wireless.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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

int nowhere_network(struct nowhere_network_info *_net) {
	struct iwreq rq;
	if (nowhere_resolve_ifname(&rq, _net->ifname) == -1) return -1;
	
	int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (fd == -1) return -1;

	// Converts numerical ip address to string address
	rq.u.addr.sa_family = AF_INET;
	if (ioctl(fd, SIOCGIFADDR, &rq) < 0) return -1;

	memset(_net->addr, 0, INET_ADDRSTRLEN);
	struct sockaddr_in *in = (struct sockaddr_in *)&rq.u.ap_addr;
	inet_ntop(AF_INET, &in->sin_addr, _net->addr, INET_ADDRSTRLEN);

	printf("{\"name\":\"wireless\",\"full_text\":\"%s %s\"},", _net->ifname, _net->addr);
	

	close(fd);
	return 0;
}
