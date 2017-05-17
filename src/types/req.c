#include "../include/types/req.h"

#include <memory.h>

http_req_t *http_req_create() {
	http_req_t *req = malloc(sizeof(http_req_t));

	return req;
}

void http_req_dispose(http_req_t * req) {

}
