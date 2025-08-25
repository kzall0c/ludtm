#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lru.h>

int main() {
	// 1. Create cache (capacity 4, 16 hash buckets)
	const unsigned int CAPACITY = 4;
	const unsigned int BUCKET_SIZE = 16;
	lru_cache_t *cache = lru_cache_create(CAPACITY, BUCKET_SIZE);

	if (!cache) {
		fprintf(stderr, "Failed to create LRU cache\n");
		return 1;
	}

	printf("LRU Cache created (Capacity: %u)\n", CAPACITY);

	// 2. Insert initial data
	printf("\n--- Phase 1: Filling the cache ---\n");
	lru_cache_put(cache, "A", 10);
	lru_cache_put(cache, "B", 20);
	lru_cache_put(cache, "C", 30);
	lru_cache_put(cache, "D", 40);

	// Expected order: MRU -> [D, C, B, A] -> LRU
	lru_cache_print(cache);

	// 3. Access data (cache hit, MRU promotion)
	printf("\n--- Phase 2: Accessing data (Cache Hit) ---\n");
	int value;
	if (lru_cache_get(cache, "B", &value)) {
		printf("Accessed 'B': %d. 'B' moves to MRU.\n", value);
	} else {
		printf("Cache Miss for 'B' (Unexpected)\n");
	}

	// Expected order: MRU -> [B, D, C, A] -> LRU
	lru_cache_print(cache);

	// 4. Update existing data
	printf("\n--- Phase 3: Updating existing data ---\n");
	printf("Updating 'A' to 15\n");
	lru_cache_put(cache, "A", 15);

	// Expected order: MRU -> [A, B, D, C] -> LRU
	lru_cache_print(cache);

	// 5. Insert beyond capacity (eviction)
	printf("\n--- Phase 4: Eviction ---\n");
	printf("Inserting 'E' (LRU item 'C' should be evicted)\n");
	lru_cache_put(cache, "E", 50);

	// Expected order: MRU -> [E, A, B, D] -> LRU
	lru_cache_print(cache);

	printf("Inserting 'F' (LRU item 'D' should be evicted)\n");
	lru_cache_put(cache, "F", 60);

	// Expected order: MRU -> [F, E, A, B] -> LRU
	lru_cache_print(cache);

	// 6. Access evicted data (cache miss)
	printf("\n--- Phase 5: Accessing evicted data (Cache Miss) ---\n");
	if (lru_cache_get(cache, "C", &value)) {
		printf("Accessed 'C': %d (Unexpected, should have been evicted)\n", value);
	} else {
		printf("Cache Miss for 'C' (Expected)\n");
	}

	if (lru_cache_get(cache, "Z", &value)) {
		 printf("Accessed 'Z': %d (Unexpected)\n", value);
	} else {
		printf("Cache Miss for 'Z' (Expected)\n");
	}

	// 7. Destroy cache
	lru_cache_destroy(cache);

	return 0;
}

