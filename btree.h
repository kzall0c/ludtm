/*
  Binary Trees

  Yunseong Kim <ysk@kzalloc.comi>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  To use binary trees you'll have to implement your own insert and search cores.
  This implementation assumes a binary search tree (BST) for ordered operations.
*/

#ifndef _BINARY_TREE_H
#define _BINARY_TREE_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

struct bt_node {
    int data;
    struct bt_node *left;
    struct bt_node *right;
};

struct bt_root {
    struct bt_node *node;
};

#define BT_ROOT (struct bt_root) { NULL, }

// Helper to create a new node
struct bt_node* bt_new_node(int data) {
    struct bt_node* node = (struct bt_node*)malloc(sizeof(struct bt_node));
    if (node) {
        node->data = data;
        node->left = node->right = NULL;
    }
    return node;
}

// Insert into BST (manipulation)
void bt_insert(struct bt_root *root, int data) {
    struct bt_node **link = &root->node;
    struct bt_node *parent = NULL;
    struct bt_node *new_node = bt_new_node(data);
    if (!new_node) return;

    while (*link) {
        parent = *link;
        if (data < parent->data) {
            link = &parent->left;
        } else if (data > parent->data) {
            link = &parent->right;
        } else {
            // Duplicate, ignore
            free(new_node);
            return;
        }
    }
    *link = new_node;
}

// Search in BST
struct bt_node* bt_search(struct bt_node *node, int data) {
    while (node) {
        if (data < node->data) {
            node = node->left;
        } else if (data > node->data) {
            node = node->right;
        } else {
            return node;
        }
    }
    return NULL;
}

// Find minimum node (for delete)
struct bt_node* bt_min_value_node(struct bt_node* node) {
    struct bt_node* current = node;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

// Delete node from BST (manipulation)
struct bt_node* bt_delete(struct bt_node* root, int data) {
    if (root == NULL) return root;

    if (data < root->data) {
        root->left = bt_delete(root->left, data);
    } else if (data > root->data) {
        root->right = bt_delete(root->right, data);
    } else {
        // Node with only one child or no child
        if (root->left == NULL) {
            struct bt_node *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            struct bt_node *temp = root->left;
            free(root);
            return temp;
        }

        // Node with two children: Get the inorder successor
        struct bt_node* temp = bt_min_value_node(root->right);

        // Copy the inorder successor's content to this node
        root->data = temp->data;

        // Delete the inorder successor
        root->right = bt_delete(root->right, temp->data);
    }
    return root;
}

// Inorder traversal
void bt_inorder(struct bt_node *node) {
    if (node) {
        bt_inorder(node->left);
        printf("%d ", node->data);
        bt_inorder(node->right);
    }
}

// Preorder traversal
void bt_preorder(struct bt_node *node) {
    if (node) {
        printf("%d ", node->data);
        bt_preorder(node->left);
        bt_preorder(node->right);
    }
}

// Postorder traversal
void bt_postorder(struct bt_node *node) {
    if (node) {
        bt_postorder(node->left);
        bt_postorder(node->right);
        printf("%d ", node->data);
    }
}

// Calculate height (validation/manipulation helper)
int bt_height(struct bt_node *node) {
    if (node == NULL) return 0;
    int left_height = bt_height(node->left);
    int right_height = bt_height(node->right);
    return (left_height > right_height ? left_height : right_height) + 1;
}

// Check if tree is balanced (validation: AVL-like check)
int bt_is_balanced(struct bt_node *node) {
    if (node == NULL) return 1;
    int left_height = bt_height(node->left);
    int right_height = bt_height(node->right);
    if (abs(left_height - right_height) > 1) return 0;
    return bt_is_balanced(node->left) && bt_is_balanced(node->right);
}

// Check if BST (validation)
int bt_is_bst_util(struct bt_node *node, int min, int max) {
    if (node == NULL) return 1;
    if (node->data < min || node->data > max) return 0;
    return bt_is_bst_util(node->left, min, node->data - 1) &&
           bt_is_bst_util(node->right, node->data + 1, max);
}

int bt_is_bst(struct bt_node *node) {
    return bt_is_bst_util(node, INT_MIN, INT_MAX);
}

// Free tree
void bt_free(struct bt_node *node) {
    if (node) {
        bt_free(node->left);
        bt_free(node->right);
        free(node);
    }
}

#endif /* _BINARY_TREE_H */

