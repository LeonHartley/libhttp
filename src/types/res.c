#include "res.h"
#include "../net/buffer/buffer.h"

#include <stdio.h>

http_res_t *http_res_create() {
	http_res_t *res = (http_res_t *) malloc(sizeof(http_res_t));

	res->header_count = 0;
	res->headers = calloc(3, sizeof(http_res_t));

	return res;
}

void http_res_add_header(char *key, char *value, http_res_t *res) {
	int header_index = res->header_count++;
	
	res->headers[header_index] = malloc(sizeof(http_header_t));

	res->headers[header_index]->key = key;
	res->headers[header_index]->value = value;
}

void http_res_send(char *str, http_res_t *res) {
	res->flush = 1;

	res->status = 200; // OK
	res->body = str;

	http_res_add_header("Content-Type", "text/html; charset=UTF-8", res);
	http_res_add_header("Server", "libhttp-1.0", res);
	http_res_add_header("Connection", "close", res);
}

typedef struct {
	http_buffer_t *buffer;
	http_res_t *res;
} http_res_state_s;

char *http_status_str(int status) {
	switch (status) {
	default:
	case 200:
		return "HTTP/1.1 200 OK";

	case 404:
		return "HTTP/1.1 404 Not Found";
	}
}

void http_res_append_header(http_header_t *header, http_res_state_s *state) {	
	http_buffer_writef(state->buffer, "%s: %s\r\n", header->key, header->value);
}

http_buffer_t *http_res_compose(http_res_t *res) {
	http_buffer_t *buffer = (http_buffer_t *) malloc(sizeof(http_buffer_t));

	buffer->position = 0;
	buffer->data = malloc(512);

	http_buffer_writef(buffer, "%s\r\n", http_status_str(res->status));

	http_res_state_s state = {
		.buffer = buffer,
		.res = res
	};

	for (int i = 0; i < res->header_count; i++) {
		http_res_append_header(res->headers[i], &state);
	}

	http_buffer_writef(buffer, "Content-Length: %i\r\n", strlen(res->body));
	http_buffer_writeln("", buffer);

	http_buffer_write(res->body + '\0', buffer);

	res->flush = 0;
	//http_res_dispose(res);
	return buffer;
}

void http_res_dispose(http_res_t *res) {
	for (int i = 0; i < res->header_count; i++) {
		free(res->headers[i]);
	}

	res->header_count = 0;
	free(res->headers);
	free(res);
}