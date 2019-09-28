
#include <stdio.h>
#include <stdlib.h>

#include "libid_config.h"
#include "id_defn_internal.h"
#include "avl.h"

avl_node_t*     id_index_tree;
avl_node_t*     data_index_tree;

void libid_init() 
{
    id_index_tree = NULL;
    data_index_tree = NULL;

    chunk_mgr_init();
}

uint32 create_id(data_ptr user_data) 
{
    // First lookup if an id is already assigned to this data (use data_index_tree)
    avl_node_t* node = avl_lookup(data_index_tree, (ulong)user_data);

    if (node != NULL) 
    {
        // Id already assigned for this user_data
        id_instance_t* id_inst = (id_instance_t *) node->key_data;
        id_inst->ref_count++;
        return id_inst->id;
    }

    // this is new data, look for available id-number in chunk-list
    id_instance_t* new_id_inst = get_free_id_inst();
    if (new_id_inst == NULL)
    {
        // FATAL ERROR
        return ERROR;
    }

    new_id_inst->ref_count++;
    new_id_inst->user_data = user_data;
    mark_id_assigned(new_id_inst->id);


    // Now, insert this new_id_inst into id_index_tree
    avl_node_t* new_id_node = calloc(sizeof(avl_node_t), 1);
    new_id_node->key = new_id_inst->id;
    new_id_node->key_data = new_id_inst;
    id_index_tree = insert_node(id_index_tree, new_id_node);

    // Now, insert this new_id_inst into data_index_tree;
    avl_node_t* new_data_node = calloc(sizeof(avl_node_t), 1);
    new_data_node->key = (ulong) user_data;
    new_data_node->key_data = new_id_inst;
    data_index_tree = insert_node(data_index_tree, new_data_node);

    return new_id_inst->id;
}

int32 delete_id(uint32 id) 
{
    // First lookup if an id is already assigned to this id (use id_index_tree)
    avl_node_t* node = avl_lookup(id_index_tree, id);

    if (node == NULL) {
        // Invalid id or no such id allocated yet.
        return ERROR;
    }
    // Id assigned for this user_data
    id_instance_t* id_inst = (id_instance_t *) node->key_data;
    if ((id_inst->ref_count == 0) || (id_inst->deleted_at != 0)) {
        // this id is already deleted. ignoring duplicate deletes
        return SUCCESS;
    }
    id_inst->ref_count--;
    if (id_inst->ref_count > 0) {
        // non-zero ref-count, no further processing required
        return SUCCESS;
    }
    // Do some book keeping and mark for garbage collection

    // 1. remove from data_index_tree
    data_index_tree = delete_node(data_index_tree, id_inst->user_data);

    // 2. Clear pointer to user-data and set deleted_at for current-time
    id_inst->user_data = NULL; // clear-up
    id_inst->deleted_at = epoch_time();

    // 3. Update the corresponding chunk allocation_flag
    mark_id_deleted(id_inst->id);

    return SUCCESS;
}

data_ptr query_id(uint32 id) 
{
    // First lookup if an id is already assigned to this id (use id_index_tree)
    avl_node_t* node = avl_lookup(id_index_tree, id);

    if (node == NULL) {
        // Invalid id or no such id allocated yet.
        return NULL;
    }
    id_instance_t* id_inst = (id_instance_t *) node->key_data;

    // check id_inst is non-null
    if (id_inst == NULL)
    {
        // FATAL ERROR
        return NULL;
    }

    return id_inst->user_data;
}
