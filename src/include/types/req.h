#pragma once

#include "../client/client.h"

#include <include/hashtable.h>

typedef struct {
	char *host;
	char *url;
	hashtable_t *headers;
} http_req_t;

http_req_t *http_req_create(http_client_t *client, char *data, int length);

void http_req_dispose(http_req_t *req);