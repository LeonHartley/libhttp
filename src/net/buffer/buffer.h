#pragma once

#include <memory.h>

typedef struct {
	int position;
	char *data;
} http_buffer_t;

http_buffer_t *http_buffer_create(size_t size);

void http_buffer_writeln(char *data, http_buffer_t *buffer);

void http_buffer_write(char *data, http_buffer_t *buffer);

void http_buffer_write_int(int i, http_buffer_t *buffer);

void http_buffer_writef(http_buffer_t *buffer, char *format, ...);

void http_buffer_dispose(http_buffer_t *buffer);