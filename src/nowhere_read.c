#include "nowhere_read.h"

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int nowhere_read(char **_buffer, size_t _size, const char *_file) {
	FILE *fp;
	if (!(fp = fopen(_file, "rb"))) return -1;

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);

	printf("%ld\n", size);

	char *ptr = malloc(size);
	fread(ptr, sizeof(char), size, fp);
	fclose(fp);
	*_buffer = ptr;
	return 0;
}

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
