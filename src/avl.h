
#ifndef _avl_h_
#define _avl_h_

#include <defs.h>

typedef struct avl_node_ {
    ulong               key;
    data_ptr            key_data;
    uint32              height;
    struct avl_node_    *left;
    struct avl_node_    *right;
} avl_node_t;

extern avl_node_t* insert_node(avl_node_t* parent_node, avl_node_t* new_node);
extern avl_node_t* delete_node(avl_node_t* parent_node, avl_node_t* new_node);

extern avl_node_t* avl_lookup(avl_node_t* parent_node, ulong key);

#endif
