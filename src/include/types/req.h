#pragma once

typedef struct {
	int placeholder;
} http_req_t;

http_req_t *http_req_create();

void http_req_dispose(http_req_t *req);