#include "../include/types/res.h"

http_res_t *http_res_create() {
	http_res_t *res = (http_res_t *) malloc(sizeof(http_res_t));

	return res;
}

void http_res_dispose(http_res_t *res) {
	// free anything else that's needed
	free(res);
}