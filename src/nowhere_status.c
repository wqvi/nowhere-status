#include <getopt.h>

static int nowhere_parse_args(int argc, char **argv) {
	static const struct option opts[] = {
		{"help", no_argument, 0, 'h'},
		{"offline", no_argument, 0, 0},
		{0, 0, 0, 0}
	};

	int c;
	int index;
	while (c = getopt_long(argc, argv, "t:i:hv", opts, &index), c != -1) {
		switch (c) {
		case 0:
			if (index == 1) {
				// offline
			}
			break;
		case 't':
			// thermal zone
			break;
		case 'i'
			// ifname
			break;
		case 'h'
			// help
			return 1;
		case 'v'
			// version
			return 1;
		default:
			return -1;
		}
	}

	return 0;
}

int main(int argc, char **argv) {
	int code = nowhere_parse_args(argc, argv);
	if (code != 0)
		return code;

	return 0;
}
