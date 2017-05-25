#include "client.h"

http_client_t *http_client_create(uv_tcp_t *socket) {
	http_client_t *client = (http_client_t *) malloc(sizeof(http_client_t));

	client->socket = socket;
	client->parser = malloc(sizeof(http_parser));
	client->res = http_res_create();

	return client;
}

void http_client_dispose(http_client_t * client) {
	//http_res_dispose(client->res);
	//free(client->res);

	http_res_dispose(client->res);
	free(client->parser);
	free(client);
}

