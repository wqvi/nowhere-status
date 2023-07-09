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
	if (ioctl(fd, SIOCGIFADDR, &rq) < 0) return -1;

	char addr[INET_ADDRSTRLEN];
	struct sockaddr_in *in = (struct sockaddr_in *)&rq.u.ap_addr;
	inet_ntop(AF_INET, &in->sin_addr, addr, INET_ADDRSTRLEN);

	_node->flags = NOWHERE_NODE_DEFAULT;
	snprintf(_node->name, NOWHERE_NAMSIZ, "wireless");
	snprintf(_node->full_text, NOWHERE_TXTSIZ, "%s %s", rq.ifr_name, addr);
	_node->alt_text[0] = '\0';

	close(fd);
	return 0;
}
