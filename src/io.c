#include <fcntl.h>
#include "nowhere_status.h"
#include <sys/stat.h>
#include <unistd.h>

int nowhere_device_read(char *_buffer, size_t _size, const char *_file) {
	int fd = open(_file, O_RDONLY);
	if (fd == -1) return -1;

	struct stat stat;
	if (fstat(fd, &stat) == -1) goto error;

	if (_size != stat.st_size) goto error;

	if (read(fd, _buffer, stat.st_size) == 0) goto error;
	
	close(fd);
	return 0;

error:
	close(fd);
	return -1;
}
