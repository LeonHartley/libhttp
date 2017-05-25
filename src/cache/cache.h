#pragma once

#include <uv.h>

void http_cache_init(uv_loop_t *loop);

void *http_cache_get(const char *key);

void http_cache_put(const char *key, void *data, int ttl);