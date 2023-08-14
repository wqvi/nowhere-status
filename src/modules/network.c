#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/wireless.h>
#include "nowhere_status.h"
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int resolve_ifname(struct iwreq *_rq) {
	struct ifaddrs *head;
	if (getifaddrs(&head)) {
		return 1;
	}
	
	for (struct ifaddrs *list = head; list != NULL; list = list->ifa_next) {
		// skip loopback device
		if (strcmp(list->ifa_name, "lo") == 0) {
			continue;
		}

		// skip any empty device names
		if (!strlen(list->ifa_name)) {
			continue;
		}

		// skip any non IPv4 devices
		if (list->ifa_addr->sa_family != AF_INET) {
			continue;
		}

		strncpy(_rq->ifr_name, list->ifa_name, IFNAMSIZ);
		_rq->u.addr.sa_family = AF_INET;
		break;
	}

	freeifaddrs(head);
	
	return 0;
}

int nowhere_network(struct node *_node) {
	struct iwreq rq;
	int fd;
	struct sockaddr_in *in;
	char addr[INET_ADDRSTRLEN];
	char essid[48] = { 0 }; // null terminated aligned buffer
	struct iw_statistics stats;
	struct iw_range range;
	int quality = 0;
	volatile size_t size = NOWHERE_TXTSIZ; // suppress compiler truncation warning

	if (resolve_ifname(&rq)) {
		return 1;
	}
	
	fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (fd == -1) {
		return 1;
	}

	// Converts numerical ip address to string address
	if (ioctl(fd, SIOCGIFADDR, &rq) < 0) {
		close(fd);
		return 1;
	}
	in = (struct sockaddr_in *)&rq.u.ap_addr;
	inet_ntop(AF_INET, &in->sin_addr, addr, INET_ADDRSTRLEN);

	// Get network essid
	rq.u.essid.pointer = essid;
	rq.u.essid.length = IW_ESSID_MAX_SIZE;
	rq.u.essid.flags = 0;
	if (ioctl(fd, SIOCGIWESSID, &rq) < 0) {
		close(fd);
		return 1;
	}

	// Get network statistics
	rq.u.data.pointer = &stats;
	rq.u.data.length = sizeof(struct iw_statistics);
	rq.u.data.flags = 0;
	if (ioctl(fd, SIOCGIWSTATS, &rq) < 0) {
		close(fd);
		return 1;
	}

	// Get network range
	rq.u.data.pointer = &range;
	rq.u.data.length = sizeof(struct iw_range);
	rq.u.data.flags = 0;
	if (ioctl(fd, SIOCGIWRANGE, &rq) < 0) {
		close(fd);
		return 1;
	}

	if (stats.qual.qual != 0) {
		quality = (stats.qual.qual * 100) / range.max_qual.qual;
	} else {
		close(fd);
		return 1;
	}

	snprintf(_node->name, NOWHERE_NAMSIZ, "wireless");
	snprintf(_node->full_text, size, "%s %s", rq.ifr_name, addr);
	snprintf(_node->alt_text, size, "%s %d%%", essid, quality);

	close(fd);
	return 0;
}
