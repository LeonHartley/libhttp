#include "router.h"

void http_router_init(http_router_t *router) {
	router->get = malloc(sizeof(hashtable_t *));
	router->post = malloc(sizeof(hashtable_t *));

	hashtable_new(&router->get);
	hashtable_new(&router->post);
}

void http_router_dispose(http_router_t *router) {
	// free the tables.
	hashtable_destroy(router->get);
	hashtable_destroy(router->post);

	free(router->get);
	free(router->post);
}

void http_router_add(int type, const char *route, http_router_cb cb, http_router_t *router) {
	hashtable_t *table = type == HTTP_GET ? router->get : router->post;

	if (hashtable_contains_key(table, (void *)route)) {
		return;
	}

	hashtable_add(table, (void *)route, cb);
}

int http_router_exec(int type, const char *route, http_client_t *client, http_req_t *req, http_router_t *router) {
	if (!hashtable_contains_key(router->get, (void *)route)) {
		// 404
		return 1;
	}
	else {
		http_router_cb callback;

		hashtable_get(router->get, (void *)route, (void**)&callback);

		((http_router_cb)callback)(client, req);
		return 0;
	}
}
