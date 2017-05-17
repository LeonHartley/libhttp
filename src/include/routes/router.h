#pragma once

#include "../client/client.h"
#include "../types/req.h"
#include "../util/hashtable.h"

typedef void(*http_router_cb)(http_client_t *client, http_req_t *req);

typedef struct {
	hashtable_t *get;
	hashtable_t *post;
} http_router_t;

void http_router_init(http_router_t *router);

void http_router_dispose(http_router_t *router);

void http_router_add(int type, const char *route, http_router_cb cb, http_router_t *router);

int http_router_exec(int type, const char *route, http_client_t *client, http_req_t *req, http_router_t *router);
