#include <stdio.h>
#include <stdlib.h>
#include <rbtree.h>

struct mytype {
    int value;
    struct rb_node node;
};

struct mytype *my_search(struct rb_root *root, int value) {
    struct rb_node *node = root->rb_node;
    struct mytype *entry;

    while (node) {
        entry = rb_entry(node, struct mytype, node);
        if (value < entry->value) {
            node = node->rb_left;
        } else if (value > entry->value) {
            node = node->rb_right;
        } else {
            return entry;
        }
    }
    return NULL;
}

int my_insert(struct rb_root *root, struct mytype *data) {
    struct rb_node **new_node = &(root->rb_node), *parent = NULL;
    struct mytype *this;

    /* Figure out where to put new node */
    while (*new_node) {
        this = rb_entry(*new_node, struct mytype, node);
        parent = *new_node;
        if (data->value < this->value) {
            new_node = &((*new_node)->rb_left);
        } else if (data->value > this->value) {
            new_node = &((*new_node)->rb_right);
        } else {
            return 0; // Already exists
        }
    }

    /* Add new node and rebalance tree. */
    rb_link_node(&data->node, parent, new_node);
    rb_insert_color(&data->node, root);

    return 1;
}

void my_delete(struct rb_root *root, int value) {
    struct mytype *data = my_search(root, value);
    if (!data) {
        return;
    }

    rb_erase(&data->node, root);
    free(data);
}

void print_tree(struct rb_root *root) {
    struct rb_node *node;
    struct mytype *entry;

    printf("In-order traversal: ");
    for (node = rb_first(root); node; node = rb_next(node)) {
        entry = rb_entry(node, struct mytype, node);
        printf("%d ", entry->value);
    }
    printf("\n");
}

int main() {
    struct rb_root mytree = RB_ROOT;
    int values[] = {5, 3, 7, 2, 4, 6, 8, 1, 9, 10};
    int num_values = sizeof(values) / sizeof(values[0]);
    int i;

    // Insert values
    printf("Inserting values: ");
    for (i = 0; i < num_values; i++) {
        struct mytype *new = malloc(sizeof(struct mytype));
        if (!new) {
            perror("malloc failed");
            return 1;
        }
        new->value = values[i];
        if (my_insert(&mytree, new)) {
            printf("%d ", values[i]);
        } else {
            free(new);
            printf("(duplicate %d skipped) ", values[i]);
        }
    }
    printf("\n");

    // Print tree after inserts
    print_tree(&mytree);

    // Test search
    int search_val = 4;
    struct mytype *found = my_search(&mytree, search_val);
    if (found) {
        printf("Searched for %d: found %d\n", search_val, found->value);
    } else {
        printf("Searched for %d: not found\n", search_val);
    }

    // Test delete
    int delete_val = 5;
    printf("Deleting %d\n", delete_val);
    my_delete(&mytree, delete_val);

    // Print tree after delete
    print_tree(&mytree);

    // Clean up remaining nodes
    while (!RB_EMPTY_ROOT(&mytree)) {
        struct rb_node *node = rb_first(&mytree);
        struct mytype *entry = rb_entry(node, struct mytype, node);
        rb_erase(node, &mytree);
        free(entry);
    }

    return 0;
}

