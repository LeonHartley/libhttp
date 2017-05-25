#include "buffer.h"

#include <memory.h>
#include <stdio.h>
#include <stdarg.h>

http_buffer_t *http_buffer_create() {
	http_buffer_t *buffer = (http_buffer_t *)malloc(sizeof(http_buffer_t));

	buffer->position = 0;
	buffer->data = malloc(512);

	return buffer;
}

void http_buffer_writeln(char *data, http_buffer_t *buffer) {
	http_buffer_write(data, buffer);

	buffer->data[buffer->position++] = '\r';
	buffer->data[buffer->position++] = '\n';
}

void http_buffer_write(char *data, http_buffer_t *buffer) {
	for (int i = 0; i < strlen(data); i++) {
		buffer->data[buffer->position++] = data[i];
	}
}

void http_buffer_writef(http_buffer_t *buffer, char *format, ...) {
	va_list args;
	va_list tmpargs;

	va_start(args, format);
	va_copy(tmpargs, args);

	// todo: calculate if we have space for the formatted data,
	//		 if we don't, resize the buffer. 
	int required = vsnprintf(NULL, 0, format, tmpargs);

	// write the formatted string to the buffer
	int bytes = vsnprintf(buffer->data + buffer->position, required + 1, format, tmpargs);
	buffer->position += bytes;

	va_end(tmpargs);
	va_end(args);
}


void http_buffer_write_int(int i, http_buffer_t *buffer) {
	memcpy(buffer->data + buffer->position, i, 4);

	buffer->position += 4;
}

void http_buffer_dispose(http_buffer_t *buffer) {
	// when we are disposing a buffer, we presume we've already
	// copied the data to a uv buffer so we can free it
	free(buffer->data);
	free(buffer);
}