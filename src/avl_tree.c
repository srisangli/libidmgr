
#include <stdio.h>
#include <defs.h>
#include "avl.h"

uint32 height(avl_node_t *n) 
{
    if (n == NULL)
        return 0;
    return n->height;
}

uint32 get_balance(avl_node_t *n) 
{
    if (n == NULL)
        return 0;

    return height(n->left) - height(n->right);
}


avl_node_t* insert_node(avl_node_t* parent_node, avl_node_t* new_node) 
{
    if (parent_node == NULL) {
        return new_node;
    }

    if (new_node->key < parent_node->key) {
        parent_node->left = insert_node(parent_node->left, new_node);
    } else if (new_node->key > parent_node->key) {
        parent_node->right = insert_node(parent_node->right, new_node);
    } else {
        // parent_node->key == new_node->key
        // overwrite key_data
        parent_node->key_data = new_node->key_data;
    }

    // TODO: Do the balancing act now
    return parent_node;
}

avl_node_t* delete_node(avl_node_t* parent_node, avl_node_t* new_node) 
{
    return parent_node;
}

avl_node_t* avl_lookup(avl_node_t* parent_node, ulong key)
{
    if (parent_node == NULL)
        return NULL;

    if (parent_node->key == key)
        return parent_node;

    if (key < parent_node->key)
        return avl_lookup(parent_node->left, key);
    else
        return avl_lookup(parent_node->right, key);
}
