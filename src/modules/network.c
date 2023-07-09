#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/wireless.h>
#include "nowhere_status.h"
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int nowhere_resolve_ifname(struct iwreq *_rq) {
	struct ifaddrs *head;
	if (getifaddrs(&head)) return -1;
	
	for (struct ifaddrs *list = head; list != NULL; list = list->ifa_next) {
		// skip loopback device
		if (strcmp(list->ifa_name, "lo") == 0) continue;
		
		// skip ipv6 interface
		if (strcmp(list->ifa_name, "sit0@NONE") == 0) continue;

		// prioritize vpn display
		if (!strcmp(list->ifa_name, "wg0")) {
			strncpy(_rq->ifr_name, list->ifa_name, IFNAMSIZ);
			break;
		}
		
		if (list->ifa_addr->sa_family == AF_INET) {
			strncpy(_rq->ifr_name, list->ifa_name, IFNAMSIZ);
			_rq->u.addr.sa_family = AF_INET;
		}
	}

	freeifaddrs(head);
	
	return 0;
}

int nowhere_network(struct node *_node) {
	struct iwreq rq;
	if (nowhere_resolve_ifname(&rq) == -1) return -1;
	
	int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (fd == -1) return -1;

	// Converts numerical ip address to string address
	if (ioctl(fd, SIOCGIFADDR, &rq) < 0) goto error;

	// Get network essid
	char essid[48]; // IW_ESSID_MAX_SIZE + 1 = 33 gross, yea yea yea the compiler is smarter than me
	rq.u.essid.pointer = essid;
	rq.u.essid.length = IW_ESSID_MAX_SIZE;
	rq.u.essid.flags = 0;

	if (ioctl(fd, SIOCGIWESSID, &rq) < 0) goto error;

	char addr[INET_ADDRSTRLEN];
	struct sockaddr_in *in = (struct sockaddr_in *)&rq.u.ap_addr;
	inet_ntop(AF_INET, &in->sin_addr, addr, INET_ADDRSTRLEN);

	snprintf(_node->name, NOWHERE_NAMSIZ, "wireless");
	// I am ok with suppressing truncation warning
	volatile size_t size = NOWHERE_TXTSIZ;
	snprintf(_node->full_text, size, "%s %s", rq.ifr_name, addr);
	snprintf(_node->alt_text, size, "%s %s", rq.ifr_name, essid);

	close(fd);
	return 0;
error:
	close(fd);
	return -1;
}
