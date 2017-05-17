#pragma comment (lib, "libuv.lib")

#include "include/net/worker.h"

#include <stdio.h>
#include <uv.h>
#include <include/http_parser.h>
#include <Windows.h>

const char *http_req = "GET /index.html HTTP/1.1\r\nHost: lib.http";

void request_handler(http_client_t *client) {
	printf("handling request....\n");
}

int main(int argc, char *argv[]) {
	http_worker_config_t *config = malloc(sizeof(http_worker_config_t));

	config->host = "0.0.0.0";
	config->port = 8080;

	http_worker_t *worker1 = http_worker_create(&request_handler, config);
	http_worker_init(worker1);

	// Wait till we're finished
	uv_thread_join(worker1->thread);
}

