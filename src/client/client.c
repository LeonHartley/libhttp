#include "../include/client/client.h"

#include <memory>

http_client_t *http_client_create(uv_tcp_t *socket) {
	http_client_t *client = (http_client_t *) malloc(sizeof(http_client_t));

	client->socket = socket;
	client->parser = malloc(sizeof(http_parser));

	return client;
}

void http_client_dispose(http_client_t * client) {
	if (client->res != NULL) {
		http_res_dispose(client->res);
	}

	if (client->req != NULL) {
		http_req_dispose(client->req);
	}

	free(client->parser);
	free(client);
}

