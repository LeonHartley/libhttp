#include "res.h"
#include "../net/buffer/buffer.h"

#include <stdio.h>

http_res_t *http_res_create() {
	http_res_t *res = (http_res_t *) malloc(sizeof(http_res_t));

	res->headers = (hashtable_t *)malloc(sizeof(hashtable_t *));
	hashtable_new(&res->headers);

	return res;
}

void http_res_send(char *str, http_res_t *res) {
	res->flush = 1;

	res->status = 200; // OK
	res->body = str;

	hashtable_add(res->headers, "Content-Type", "text/html; charset=UTF-8");
	hashtable_add(res->headers, "Server", "libhttp-1.0");
	hashtable_add(res->headers, "Connection", "close");
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

void http_res_append_header(void *key, void *data) {
	// find the header, append the header to the buffer.
	http_res_state_s *state = (http_res_state_s *)data;
	char *val;
	
	hashtable_get(state->res->headers, key, &val);

	if (val != NULL) {
		http_buffer_write(key, state->buffer);
		http_buffer_write(": ", state->buffer);
		http_buffer_writeln(val, state->buffer);
	}
}

http_buffer_t *http_res_compose(http_res_t *res) {
	http_buffer_t *buffer = (http_buffer_t *)malloc(sizeof(http_buffer_t));

	buffer->position = 0;
	buffer->data = malloc(512);

	http_buffer_writeln(http_status_str(res->status), buffer);

	http_res_state_s state;
	state.buffer = buffer;
	state.res = res;

	hashtable_foreach_key(res->headers, &http_res_append_header, &state);

	http_buffer_writef(buffer, "Content-Length: %i\r\n", strlen(res->body));
	http_buffer_writeln("", buffer);

	http_buffer_write(res->body + '\0', buffer);

	res->flush = 0;
	//http_res_dispose(res);
	return buffer;
}

void http_res_dispose(http_res_t *res) {
	// free anything else that's needed
	hashtable_destroy(res->headers);
}