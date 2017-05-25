#include "cache.h"

#include "../util/hashtable.h"

#include <uv.h>
#include <memory.h>

uv_rwlock_t cache_lock;
uv_timer_t timer;
hashtable_t *cache;

int removals_counter = 0;

typedef struct {
	char *key;
	int time;
	int ttl;
	void *data;
} http_cache_entry_s;

http_cache_entry_s **removals;

void http_cache_expire_if_needed(void *value) {
	int current_time = time(NULL);

	http_cache_entry_s *entry = (http_cache_entry_s *)value;

	if ((entry->time + entry->ttl) <= current_time) {
		removals[removals_counter++] = entry;
	}
}

void http_cache_process() {
	uv_rwlock_wrlock(&cache_lock);

	removals_counter = 0;
	removals = calloc(hashtable_capacity(cache), sizeof(http_cache_entry_s));
	
	hashtable_foreach_value(cache, &http_cache_expire_if_needed);

	for (int i = 0; i < removals_counter; i++) {
		http_cache_entry_s *entry = removals[i];

		hashtable_remove(cache, entry->key, &entry);

		free(entry->data);
		free(entry);
	}

	free(removals);

	uv_rwlock_wrunlock(&cache_lock);
}

void http_cache_init(uv_loop_t *loop) {
	uv_rwlock_init(&cache_lock);

	cache = malloc(sizeof(hashtable_t *));
	hashtable_new(&cache);
	
	uv_timer_init(loop, &timer);
	uv_timer_start(&timer, &http_cache_process, 1000, 1000);
}

void *http_cache_get(const char *key) {
	void *ptr = NULL;

	uv_rwlock_rdlock(&cache_lock);

	hashtable_get(cache, key, &ptr);

	uv_rwlock_rdunlock(&cache_lock);

	return ptr;
}

void http_cache_put(const char *key, void *data, int ttl) {
	uv_rwlock_wrlock(&cache_lock);

	http_cache_entry_s *entry = (http_cache_entry_s *) malloc(sizeof(http_cache_entry_s));

	entry->key = key;
	entry->time = time(NULL);
	entry->data = data;
	entry->ttl = ttl;
	
	hashtable_add(cache, key, entry);
	uv_rwlock_wrunlock(&cache_lock);
}