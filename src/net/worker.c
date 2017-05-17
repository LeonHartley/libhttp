#include "../include/net/worker.h"

#include <stdio.h>

void http_worker_alloc_buffer(uv_handle_t *handle, ssize_t size, uv_buf_t *buffer) {
	buffer->base = malloc(size);
	buffer->len = size;
}

void http_worker_on_close(uv_stream_t *handle) {
	
}

void http_worker_read(uv_stream_t *handle, ssize_t read, const uv_buf_t *buffer) {
	// read the entire request and parse it.
	// once the message has been parsed, we want to dispatch
	// the request to an external handler.

	if (read < 0) {
		// disconnect
		return;
	}

	char *data = malloc(read);
	memcpy(data, buffer->base, read);

	printf("data received %s\n", data);
	
	free(data);
	free(buffer->base);
}

void http_worker_connection(uv_stream_t *server, int status) {
	uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));

	uv_tcp_init(server->loop, client);

	int result = uv_accept(server, (uv_stream_t *)client);

	if (!result) {
		uv_read_start((uv_stream_t *)client, &http_worker_alloc_buffer, &http_worker_read);
	}
	else {
		printf("Failed to accept client %s", uv_strerror(result));
	}
}

void http_worker(void *arg) {
	http_worker_t *worker = (http_worker_t *)arg;

	// start tcp server
	uv_tcp_t server;
	struct sockaddr_in bind_addr;

	uv_tcp_init(worker->loop, &server);
	uv_ip4_addr(worker->config->host, worker->config->port, &bind_addr);
	uv_tcp_bind(&server, (const struct sockaddr *) &bind_addr, 0);
	uv_listen((uv_stream_t *)&server, 128, &http_worker_connection);
	
	int result = uv_run(worker->loop, UV_RUN_DEFAULT);

	if (result) {
		printf("Failed to start HTTP worker %s", uv_strerror(result));
	}
}

http_worker_t *http_worker_create(http_request_handler_cb cb, http_worker_config_t *config) {
	http_worker_t *worker = (http_worker_t *)malloc(sizeof(http_worker_t));

	worker->config = config;
	worker->handler = cb;
	worker->loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
	worker->thread = (uv_thread_t *)malloc(sizeof(uv_thread_t));

	return worker;
}

void http_worker_init(http_worker_t *worker) {
	uv_loop_init(worker->loop);

	worker->loop->data = worker;

	uv_thread_create(worker->thread, &http_worker, worker);
}