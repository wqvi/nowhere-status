#ifndef NOWHERE_READ_H
#define NOWHERE_READ_H

#include <stddef.h>

int nowhere_read(char **_buffer, size_t _size, const char *_file);

int nowhere_device_read(char *_buffer, size_t _size, const char *_file);

#endif // NOWHERE_READ_H
