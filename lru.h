#ifndef LRU_H
#define LRU_H

#include <stddef.h>
#include <hashmap.h>

/**
 * Structure representing an LRU (Least Recently Used) cache.
 */
typedef struct lru_cache {
	unsigned int capacity;	  // Maximum number of items the cache can store
	unsigned int count;		 // Current number of items in the cache
	unsigned int bucket_size;   // Number of hash buckets for O(1) lookup

	// Doubly linked list to track usage order.
	// The most recently used (MRU) item is at lru_head.next.
	// The least recently used (LRU) item is at lru_head.prev.
	struct list_head lru_head;

	// Hash table buckets for O(1) key lookup (array of hlist_head)
	struct hlist_head *buckets;
} lru_cache_t;

/**
 * Create and initialize a new LRU cache.
 * @param capacity	Maximum number of items to store.
 * @param bucket_size Size of the internal hash table.
 * @return Pointer to the newly created LRU cache, or NULL on failure.
 */
lru_cache_t* lru_cache_create(unsigned int capacity, unsigned int bucket_size);

/**
 * Destroy the LRU cache and free all associated memory.
 * @param cache LRU cache instance to destroy.
 */
void lru_cache_destroy(lru_cache_t *cache);

/**
 * Retrieve a value from the cache.
 * If the key exists, the item is marked as MRU.
 * @param cache LRU cache instance.
 * @param key   Key to look up.
 * @param value Output pointer for the retrieved value.
 * @return 1 if the key exists (cache hit), 0 if not found (cache miss).
 */
int lru_cache_get(lru_cache_t *cache, const char *key, int *value);

/**
 * Insert or update a key-value pair in the cache.
 * The item is marked as MRU. If the cache is full, the LRU item is evicted.
 * @param cache LRU cache instance.
 * @param key   Key string.
 * @param value Value to store.
 * @return 0 on success, -1 on memory allocation failure.
 */
int lru_cache_put(lru_cache_t *cache, const char *key, int value);

/**
 * Print the current state of the cache (for debugging).
 * Items are printed in MRU -> LRU order.
 * @param cache LRU cache instance.
 */
void lru_cache_print(lru_cache_t *cache);

// Structure representing an individual node in the cache.
// Uses intrusive list design for both the hash table and LRU list.
typedef struct lru_node {
	char *key;
	int value;  // Stores integer values (based on hashmap.c example)

	// Hash map linkage (via hlist)
	struct hlist_node h_node;

	// LRU list linkage (via list_head)
	struct list_head lru_list;
} lru_node_t;

/**
 * Internal helper: Look up a node in the hash map by key.
 */
static lru_node_t* cache_lookup(lru_cache_t *cache, const char *key) {
	unsigned long hash = hash_function(key);
	unsigned int index = hash % cache->bucket_size;

	struct hlist_head *head = &cache->buckets[index];
	struct hlist_node *pos;
	lru_node_t *entry;

	// Traverse the hlist for this bucket.
	hlist_for_each_entry(entry, pos, head, h_node) {
		if (strcmp(entry->key, key) == 0) {
			return entry;
		}
	}
	return NULL;
}

/**
 * Internal helper: Remove a node safely from both lists and free its memory.
 */
static void free_node(lru_node_t *node) {
	// 1. Remove from hash map (if linked)
	if (node->h_node.pprev) {
		 hlist_del(&node->h_node);
	}

	// 2. Remove from LRU list (if linked)
	if (node->lru_list.next != &node->lru_list) {
		list_del(&node->lru_list);
	}

	// 3. Free memory
	free(node->key);
	free(node);
}

/*
 * ====================================================================================
 * Public API Implementation
 * ====================================================================================
 */

lru_cache_t* lru_cache_create(unsigned int capacity, unsigned int bucket_size) {
	if (capacity == 0 || bucket_size == 0) return NULL;

	// 1. Allocate the cache structure
	lru_cache_t *cache = (lru_cache_t*)malloc(sizeof(lru_cache_t));
	if (!cache) {
		perror("malloc lru_cache_t");
		return NULL;
	}

	// 2. Allocate hash buckets
	cache->buckets = (struct hlist_head*)malloc(sizeof(struct hlist_head) * bucket_size);
	if (!cache->buckets) {
		perror("malloc buckets");
		free(cache);
		return NULL;
	}

	// 3. Initialize attributes
	cache->capacity = capacity;
	cache->bucket_size = bucket_size;
	cache->count = 0;

	// 4. Initialize LRU list head
	INIT_LIST_HEAD(&cache->lru_head);

	// 5. Initialize all hash buckets
	for (unsigned int i = 0; i < bucket_size; i++) {
		INIT_HLIST_HEAD(&cache->buckets[i]);
	}

	return cache;
}

void lru_cache_destroy(lru_cache_t *cache) {
	if (!cache) return;

	lru_node_t *entry, *tmp;

	// Use the safe iteration macro since nodes are freed during traversal.
	list_for_each_entry_safe(entry, tmp, &cache->lru_head, lru_list) {
		free_node(entry);
	}

	free(cache->buckets);
	free(cache);
}

int lru_cache_get(lru_cache_t *cache, const char *key, int *value) {
	// 1. Lookup the key in the hash map
	lru_node_t *node = cache_lookup(cache, key);

	if (node) {
		// Cache hit: Move node to MRU position
		list_move(&node->lru_list, &cache->lru_head);

		// Return the stored value
		*value = node->value;
		return 1;
	}

	// Cache miss
	return 0;
}

int lru_cache_put(lru_cache_t *cache, const char *key, int value) {
	// 1. Check if the key already exists
	lru_node_t *node = cache_lookup(cache, key);

	if (node) {
		// Key exists: Update value and move to MRU
		node->value = value;
		list_move(&node->lru_list, &cache->lru_head);
		return 0;
	}

	// 2. Insert a new entry

	// 2.1. Evict LRU if capacity exceeded
	if (cache->count >= cache->capacity) {
		// The LRU node is at the tail (lru_head.prev)
		lru_node_t *lru_node = list_entry(cache->lru_head.prev, lru_node_t, lru_list);

		// Remove and free the LRU node
		free_node(lru_node);
		cache->count--;
	}

	// 2.2. Allocate a new node
	lru_node_t *new_node = (lru_node_t*)malloc(sizeof(lru_node_t));
	if (!new_node) {
		perror("malloc lru_node_t");
		return -1;
	}

	new_node->key = strdup(key);
	if (!new_node->key) {
		perror("strdup key");
		free(new_node);
		return -1;
	}
	new_node->value = value;

	// Initialize list nodes
	INIT_HLIST_NODE(&new_node->h_node);
	INIT_LIST_HEAD(&new_node->lru_list);

	// 2.3. Insert into hash table
	unsigned long hash = hash_function(key);
	unsigned int index = hash % cache->bucket_size;
	hlist_add_head(&new_node->h_node, &cache->buckets[index]);

	// 2.4. Insert into LRU list at MRU position
	list_add(&new_node->lru_list, &cache->lru_head);

	cache->count++;

	return 0;
}

void lru_cache_print(lru_cache_t *cache) {
	if (!cache) return;

	printf("\n--- LRU Cache State (Count: %u / Capacity: %u) ---\n", cache->count, cache->capacity);
	printf("MRU -> ");

	lru_node_t *entry;

	list_for_each_entry(entry, &cache->lru_head, lru_list) {
		printf("['%s': %d] -> ", entry->key, entry->value);
	}

	printf("LRU\n");
	printf("-----------------------------------------------------\n\n");
}

#endif /* LRU_H */

