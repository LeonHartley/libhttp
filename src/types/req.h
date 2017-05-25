#pragma once

#include "../client/client.h"
#include "header.h"

typedef struct {
	int type;
	char *host;
	char *url;

	int header_count;
	http_header_t **headers;
} http_req_t;

http_req_t *http_req_create(http_client_t *client, char *data, int length);

void http_req_dispose(http_req_t *req);