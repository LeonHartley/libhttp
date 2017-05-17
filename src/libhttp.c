#pragma comment (lib, "libuv.lib")

#include "include/net/worker.h"
#include "include/routes/router.h"

#include <stdio.h>
#include <uv.h>
#include <memory.h>

void index_handler(http_client_t *client, http_req_t *req) {
	printf("Index was handled.\n");
}

void hello_handler(http_client_t *client, http_req_t *req) {
	printf("Hello was handled.\n");
}

int main(int argc, char *argv[]) {
	http_router_t router;

	http_router_init(&router);

	http_router_add(HTTP_GET, "/", &index_handler, &router);
	http_router_add(HTTP_GET, "/index", &index_handler, &router);
	http_router_add(HTTP_GET, "/hello", &hello_handler, &router);

	http_worker_config_t config;

	config.host = "0.0.0.0";
	config.port = 8080;
	config.router = &router;
	
	http_worker_t *worker1 = http_worker_create(NULL, &config);
	http_worker_init(worker1);

	// Wait till we're finished
	uv_thread_join(worker1->thread);
}

