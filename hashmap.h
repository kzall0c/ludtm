#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <list.h>

// 1. Data Structure Definition

/**
 * Node structure to be stored in the hashmap (Key: string, Value: integer).
 */
struct hash_node {
	char *key;
	int value;
	struct hlist_node h_node; // Embedded hlist node
};

/**
 * Hashmap structure definition.
 */
struct hash_map {
	struct hlist_head *buckets; // Bucket array (dynamically allocated)
	unsigned int size;		  // Bucket size
};

// 2. Hash Function

/**
 * Simple djb2 hash function (for strings).
 */
static unsigned long hash_function(const char *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

// 3. Hashmap Operation Functions

/**
 * Create and initialize hashmap
 * @param size Bucket size
 * @return Pointer to created hash_map, NULL on failure
 */
struct hash_map* hash_map_create(unsigned int size) {
	if (size == 0) return NULL;

	// 1. Allocate hashmap structure
	struct hash_map *map = (struct hash_map*)malloc(sizeof(struct hash_map));
	if (!map) {
		perror("malloc hash_map");
		return NULL;
	}

	// 2. Allocate bucket array
	map->buckets = (struct hlist_head*)malloc(sizeof(struct hlist_head) * size);
	if (!map->buckets) {
		perror("malloc buckets");
		free(map);
		return NULL;
	}

	// 3. Initialization
	map->size = size;
	for (unsigned int i = 0; i < size; i++) {
		INIT_HLIST_HEAD(&map->buckets[i]);
	}

	return map;
}

/**
 * Search for node using key (internal helper function)
 * @param map Hashmap pointer
 * @param key Key to search
 * @return Found hash_node pointer, NULL if not found
 */
static struct hash_node* hash_map_lookup(struct hash_map *map, const char *key) {
	unsigned long hash = hash_function(key);
	unsigned int index = hash % map->size;

	struct hlist_head *head = &map->buckets[index];
	struct hlist_node *pos;
	struct hash_node *entry;

	// Traverse bucket using hlist_for_each_entry macro
	// hlist_for_each_entry(tpos, pos, head, member)
	hlist_for_each_entry(entry, pos, head, h_node) {
		if (strcmp(entry->key, key) == 0) {
			return entry;
		}
	}

	return NULL;
}

/**
 * Retrieve value using key
 * @param map Hashmap pointer
 * @param key Key to search
 * @param value Pointer to store the value
 * @return 1 if key found, 0 on failure
 */
int hash_map_get(struct hash_map *map, const char *key, int *value) {
	struct hash_node *node = hash_map_lookup(map, key);
	if (node) {
		*value = node->value;
		return 1;
	}
	return 0;
}


/**
 * Insert key-value pair (update value if key already exists)
 * @param map Hashmap pointer
 * @param key Key to insert
 * @param value Value to insert
 * @return 0 on success, -1 on failure (memory allocation error, etc.)
 */
int hash_map_insert(struct hash_map *map, const char *key, int value) {
	// 1. Check if key already exists
	struct hash_node *entry = hash_map_lookup(map, key);
	if (entry) {
		// If exists, update the value
		entry->value = value;
		return 0;
	}

	// 2. Create new node
	entry = (struct hash_node*)malloc(sizeof(struct hash_node));
	if (!entry) {
		perror("malloc hash_node");
		return -1;
	}

	// 3. Copy key string (using strdup) and set value
	entry->key = strdup(key);
	if (!entry->key) {
		perror("strdup key");
		free(entry);
		return -1;
	}
	entry->value = value;
	INIT_HLIST_NODE(&entry->h_node);

	// 4. Calculate bucket index
	unsigned long hash = hash_function(key);
	unsigned int index = hash % map->size;
	struct hlist_head *head = &map->buckets[index];

	// 5. Add node to the head of the bucket list (O(1))
	hlist_add_head(&entry->h_node, head);

	return 0;
}

/**
 * Delete node using key
 * @param map Hashmap pointer
 * @param key Key to delete
 */
void hash_map_delete(struct hash_map *map, const char *key) {
	// 1. Find node
	struct hash_node *entry = hash_map_lookup(map, key);

	if (entry) {
		// 2. Remove from list (using hlist_del)
		// hlist_del operates in O(1) thanks to the pprev pointer.
		hlist_del(&entry->h_node);

		// 3. Free memory
		free(entry->key); // Free key allocated with strdup
		free(entry);
	}
}

/**
 * Print hashmap contents (for debugging)
 */
void hash_map_print(struct hash_map *map) {
	printf("\n--- HashMap Contents (Bucket Size: %u) ---\n", map->size);
	int count = 0;
	for (unsigned int i = 0; i < map->size; i++) {
		struct hlist_head *head = &map->buckets[i];
		if (!hlist_empty(head)) {
			printf("Bucket[%u]: ", i);
			struct hlist_node *pos;
			struct hash_node *entry;

			hlist_for_each_entry(entry, pos, head, h_node) {
				printf("['%s': %d] -> ", entry->key, entry->value);
				count++;
			}
			printf("NULL\n");
		}
	}
	if (count == 0) {
		printf("Map is empty.\n");
	}
	printf("Total elements: %d\n", count);
	printf("--------------------------------------------\n\n");
}

/**
 * Destroy hashmap and free all memory
 * @param map Hashmap pointer
 */
void hash_map_destroy(struct hash_map *map) {
	if (!map) return;

	printf("Destroying hash map...\n");
	int count = 0;

	// Traverse all buckets
	for (unsigned int i = 0; i < map->size; i++) {
		struct hlist_head *head = &map->buckets[i];
		struct hlist_node *pos, *n; // n is temporary storage for safe traversal
		struct hash_node *entry;

		// Delete and free all nodes in the bucket (use safe macro since deleting during traversal)
		// hlist_for_each_entry_safe(tpos, pos, n, head, member)
		hlist_for_each_entry_safe(entry, pos, n, head, h_node) {
			hlist_del(&entry->h_node);
			free(entry->key);
			free(entry);
			count++;
		}
	}

	// Free bucket array and map structure
	free(map->buckets);
	free(map);
	printf("Freed %d elements.\n", count);
}

#endif /* HASHMAP_H */

