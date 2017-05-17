#pragma once

#include <uv.h>

typedef struct {
	uv_tcp_t *socket;
} http_res_t;

http_res_t *http_res_create();

void http_res_dispose(http_res_t *res);