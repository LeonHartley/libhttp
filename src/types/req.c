#include "req.h"

#include <memory.h>
#include <stdio.h>

int http_parser_on_header_field(http_parser *parser, const char *at, size_t length) {
	return 0;
}

int http_parser_on_header_value(http_parser *parser, const char *at, size_t length) {
	return 0;
}

int http_parser_on_url(http_parser *parser, const char *at, size_t length) {
	http_req_t *req = (http_req_t *)parser->data;

	req->url = malloc(length + 1);
	memcpy(req->url, at, length + 1);

	req->url[length] = '\0';
	return 0;
}

int http_parser_on_body(http_parser *parser, const char *at, size_t length) {
	return 0;
}

http_parser_settings http_req_parser_settings() {
	http_parser_settings settings;

	http_parser_settings_init(&settings);

	settings.on_url = &http_parser_on_url;
	settings.on_body = &http_parser_on_body;
	settings.on_header_field = &http_parser_on_header_field;
	settings.on_header_value = &http_parser_on_header_value;

	return settings;
}	

http_req_t *http_req_create(http_client_t *client, char *data, int length) {
	http_req_t *req = malloc(sizeof(http_req_t));
		
	http_parser_init(client->parser, HTTP_REQUEST);

	client->parser->data = req;

	http_parser_settings settings = http_req_parser_settings();

	int parsed = http_parser_execute(client->parser, &settings, data, length);

	if (parsed != length) {
		printf("Failed to parse http request, error: %s: %s\n", http_errno_name(client->parser->http_errno), http_errno_description(client->parser->http_errno));
	}
	else {
		printf("Parsed http request, length: %i\n", parsed);
		
		req->type = client->parser->type;
	}
	
	return req;
}

void http_req_dispose(http_req_t * req) {
	// free anything allocated in the request.
	free(req->url);
}
