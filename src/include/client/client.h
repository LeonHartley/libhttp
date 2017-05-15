#pragma once

#include "../types/res.h"
#include "../types/req.h"

#include <include/http_parser.h>
#include <uv.h>

typedef struct {
	uv_tcp_t *socket;
	http_parser *parser;
	http_req_t *req;
	http_res_t *res;
} http_client_t;

http_client_t *http_client_create(uv_tcp_t *socket);

void http_client_dispose(http_client_t *client);