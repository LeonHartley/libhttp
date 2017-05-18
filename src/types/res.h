#pragma once

#include "../net/buffer/buffer.h"
#include "../util/hashtable.h"

#include <stdio.h>
#include <uv.h>

typedef struct {
	int status;
	hashtable_t *headers;
	char *body;

	int flush;
} http_res_t;

http_res_t *http_res_create();

void http_res_send(char *str, http_res_t *res);

http_buffer_t *http_res_compose(http_res_t *res);

void http_res_dispose(http_res_t *res);