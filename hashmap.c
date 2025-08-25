#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <list.h>
#include <hashmap.h>

int main() {
	// 1. Create hashmap (bucket size 16)
	struct hash_map *map = hash_map_create(16);
	if (!map) {
		return 1;
	}

	printf("--- Inserting data ---\n");
	hash_map_insert(map, "Alice", 25);
	hash_map_insert(map, "Bob", 30);
	hash_map_insert(map, "Charlie", 35);
	hash_map_insert(map, "David", 40);
	hash_map_insert(map, "Eve", 45);

	hash_map_print(map);

	// 2. Data update
	printf("--- Updating 'Alice' to 26 ---\n");
	hash_map_insert(map, "Alice", 26);

	hash_map_print(map);

	// 3. Data search
	printf("--- Searching data ---\n");
	int value;

	if (hash_map_get(map, "Bob", &value)) {
		printf("Found 'Bob': %d\n", value);
	} else {
		printf("'Bob' not found\n");
	}

	if (hash_map_get(map, "George", &value)) {
		printf("Found 'George': %d\n", value);
	} else {
		printf("'George' not found\n");
	}

	// 4. Data deletion
	printf("\n--- Deleting 'Charlie' ---\n");
	hash_map_delete(map, "Charlie");
	hash_map_print(map);

	printf("--- Deleting 'Eve' ---\n");
	hash_map_delete(map, "Eve");
	hash_map_print(map);

	// 5. Destroy hashmap
	hash_map_destroy(map);
	// The map pointer is now invalid.

	return 0;
}

