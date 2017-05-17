#include "../include/net/worker.h"

#include <stdio.h>

const char *DEFAULT_RESPONSE = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=UTF-8\r\n\
Content-Encoding: UTF-8\r\n\
Content-Length: 16\r\n\
Server: libhttp-1.0\r\n\
Connection: close\r\n\
\r\n\
send buffer test";

void http_worker_alloc_buffer(uv_handle_t *handle, ssize_t size, uv_buf_t *buffer) {
	buffer->base = malloc(size);
	buffer->len = size;
}

void http_worker_on_close(uv_stream_t *handle) {
	// free anything allocated
}

void http_worker_on_write(uv_write_t *req, int status) {
	if (status) {
		printf("Failed to write to client %s", uv_strerror(status));
		return;
	}
}

void http_worker_close_on_write(uv_write_t *req, int status) {
	http_worker_on_write(req, status);

	uv_close((uv_handle_t *)req->handle, &http_worker_on_close);
}

/*
	// send the demo response
	uv_write_t *write = malloc(sizeof(uv_write_t));
	uv_buf_t wbuf = uv_buf_init(malloc(strlen(DEFAULT_RESPONSE)), strlen(DEFAULT_RESPONSE));

	memcpy(wbuf.base, DEFAULT_RESPONSE, wbuf.len);

	write->handle = handle;
	write->data = buffer;

	uv_write(write, handle, &wbuf, 1, &http_worker_close_on_write);

	free(data);
	free(buffer->base);
	*/

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

	http_client_t *client = http_client_create((uv_tcp_t *)handle);
	http_req_t *request = http_req_create(client, data, read);

	http_worker_t *worker = (http_worker_t *)handle->loop->data;

	http_router_exec(request->type, request->url, client, request, worker->config->router);
	http_req_dispose(request);
}

void http_worker_connection(uv_stream_t *server, int status) {
	uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));

	uv_tcp_init(server->loop, client);

	int result = uv_accept(server, (uv_stream_t *)client);

	if (!result) {
		uv_read_start((uv_stream_t *)client, &http_worker_alloc_buffer, &http_worker_read);
	}
	else {
		printf("Failed to accept client %\ns", uv_strerror(result));
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