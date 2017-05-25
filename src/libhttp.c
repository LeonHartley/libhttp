#pragma comment (lib, "libuv.lib")

#include "cache/cache.h"
#include "net/worker.h"
#include "routes/router.h"

#include <stdio.h>
#include <uv.h>

#ifdef _WIN32
#include <windows.h>
#endif

void libhttp_handler(http_client_t *client, http_req_t *req) {
	http_res_send("<h3>libhttp-1.0</h3>Powered by libhttp-1.0, a minimal asynchronous HTTP server written in C<br /><br /> \
		Source code available on <a href=\"https://github.com/leonhartley/libhttp\">GitHub</a>", client->res);
}

void on_request(http_client_t *client, http_req_t *req) {
	printf("requesting %s\n", req->url);

}

typedef struct {
	char *data;
} test_s;

int main(int argc, char *argv[]) {

#ifdef _WIN32
	SetConsoleTitle("libhttp-1.0");
#endif

	http_router_t router;
	http_router_init(&router);

	http_router_add(HTTP_GET, "/libhttp", &libhttp_handler, &router);

	http_worker_config_t config;

	config.host = "0.0.0.0";
	config.port = 8080;
	config.router = &router;
	
	http_worker_t *worker1 = http_worker_create(&on_request, &config);
	http_worker_init(worker1);

	http_cache_init(worker1->loop);
	
	test_s *test = (test_s *)malloc(sizeof(test_s));

	// cache for 5 seconds
	http_cache_put("test", test, 5);

	// Wait till we're finished
	uv_thread_join(worker1->thread);
}

