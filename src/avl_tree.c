
#include  <stdlib.h>
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

//
// API to insert a key, key_data into given avl-tree (given parent)
//
avl_node_t* insert_node(avl_node_t* parent_node, ulong key, data_ptr key_data)
{
    if (parent_node == NULL) {
        avl_node_t* new_node = calloc(sizeof(avl_node_t), 1);
        new_node->key = key;
        new_node->key_data = key_data;
        return new_node;
    }

    if (key < parent_node->key) {
        parent_node->left = insert_node(parent_node->left, key, key_data);
    } else if (key > parent_node->key) {
        parent_node->right = insert_node(parent_node->right, key, key_data);
    } else {
        // parent_node->key == new_node->key
        // overwrite key_data
        parent_node->key_data = key_data;
    }

    // TODO: Do the balancing act now
    return parent_node;
}

//
// Internal API to return node with minimum value
//
avl_node_t* min_key_node(avl_node_t* node)
{
    while (node != NULL && node->left != NULL)
    {
        node = node->left;
    }
    return node;
}

//
// API to delete a node given key
//
avl_node_t* delete_node(avl_node_t* parent_node, ulong key) 
{
    if (parent_node == NULL)
    {
        return NULL; // nothing to do
    }

    if (key < parent_node->key) 
    {
        // key is part of left sub-tree
        parent_node->left = delete_node(parent_node->left, key);
    } 
    else if (key > parent_node->key) 
    {
        // key is part of left right-tree
        parent_node->right = delete_node(parent_node->right, key);
    }
    else
    {
        // this is the node to be deleted
        if (parent_node->left == NULL)
        {
            // no left-child
            avl_node_t* t = parent_node->right;
            free(parent_node);
            return t;
        }
        else if (parent_node->right == NULL)
        {
            // no right-child
            avl_node_t* t = parent_node->left;
            free(parent_node);
            return t;
        }
        else
        {
            // this node has both left and right children
            // look for next successor (inorder)
            avl_node_t* min_node = min_key_node(parent_node->right);
            parent_node->key = min_node->key;
            parent_node->key_data = min_node->key_data;
            
            // delete min_node in the right sub-tree
            parent_node->right = delete_node(parent_node->right, min_node->key);
        }
    }
    return parent_node;
}

//
// API to lookup key (and key-data), given key
//
data_ptr avl_lookup(avl_node_t* parent_node, ulong key)
{
    if (parent_node == NULL)
        return NULL;

    if (parent_node->key == key)
        return parent_node->key_data;

    if (key < parent_node->key)
        return avl_lookup(parent_node->left, key);

    return avl_lookup(parent_node->right, key);
}
