#pragma once

#include <uv.h>
#include <memory.h>

#include "../routes/router.h"
#include "../client/client.h"
#include "../types/req.h"

typedef void(*http_request_handler_cb) (http_client_t *client, http_req_t *req);

typedef struct {
	char *host;
	int port;
	http_router_t *router;
} http_worker_config_t;

typedef struct {
	http_worker_config_t *config;
	http_request_handler_cb handler;
	uv_loop_t *loop;
	uv_thread_t *thread;
} http_worker_t;

http_worker_t *http_worker_create(http_request_handler_cb cb, http_worker_config_t *config);

void http_worker_init(http_worker_t *worker);