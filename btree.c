#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <btree.h>

int main() {
    struct bt_root mytree = BT_ROOT;
    int values[] = {5, 3, 7, 2, 4, 6, 8, 1, 9, 10};
    int num_values = sizeof(values) / sizeof(values[0]);
    int i;

    // Insert values (manipulation)
    printf("Inserting values into BST: ");
    for (i = 0; i < num_values; i++) {
        bt_insert(&mytree, values[i]);
        printf("%d ", values[i]);
    }
    printf("\n");

    // Traversals
    printf("Inorder traversal: ");
    bt_inorder(mytree.node);
    printf("\n");

    printf("Preorder traversal: ");
    bt_preorder(mytree.node);
    printf("\n");

    printf("Postorder traversal: ");
    bt_postorder(mytree.node);
    printf("\n");

    // Search
    int search_val = 4;
    struct bt_node *found = bt_search(mytree.node, search_val);
    if (found) {
        printf("Searched for %d: found %d\n", search_val, found->data);
    } else {
        printf("Searched for %d: not found\n", search_val);
    }

    // Delete (manipulation)
    int delete_val = 5;
    printf("Deleting %d\n", delete_val);
    mytree.node = bt_delete(mytree.node, delete_val);

    // Traversals after delete
    printf("Inorder after delete: ");
    bt_inorder(mytree.node);
    printf("\n");

    // Validation
    printf("Tree height: %d\n", bt_height(mytree.node));
    printf("Is balanced: %s\n", bt_is_balanced(mytree.node) ? "Yes" : "No");
    printf("Is BST: %s\n", bt_is_bst(mytree.node) ? "Yes" : "No");

    // Clean up
    bt_free(mytree.node);

    return 0;
}

