
#include <stdio.h>
#include <stdlib.h>

#include "libid_config.h"
#include "id_defn_internal.h"
#include "avl.h"

avl_node_t*     id_index_tree;
avl_node_t*     data_index_tree;

void libid_init(uint32 gc_timeout) 
{
    id_index_tree = NULL;
    data_index_tree = NULL;

    if (gc_timeout == 0) // use default
        gc_timeout = GC_TIMEOUT;

    chunk_mgr_init(gc_timeout);
}

uint32 create_id(data_ptr user_data) 
{
    // First lookup if an id is already assigned to this data (use data_index_tree)
    id_instance_t* id_inst = avl_lookup(data_index_tree, (ulong)user_data);

    // check before proceeding
    if ((id_inst != NULL) && (id_inst->ref_count > 0) && (id_inst->deleted_at == 0))
    {
        // Id already assigned for this user_data
        mark_id_assigned(id_inst->id);
        return id_inst->id;
    }

    // this is new data, look for available id-number in chunk-list
    id_instance_t* new_id_inst = get_free_id_inst();
    if (new_id_inst == NULL)
    {
        // FATAL ERROR
        return ERROR;
    }

    new_id_inst->user_data = user_data;
    mark_id_assigned(new_id_inst->id);


    // Now, insert this new_id_inst into id_index_tree
    id_index_tree = insert_node(id_index_tree, 
                                (ulong)new_id_inst->id, (data_ptr)new_id_inst);

    // Now, insert this new_id_inst into data_index_tree;
    data_index_tree = insert_node(data_index_tree, 
                                  (ulong)user_data, (data_ptr)new_id_inst);

    return new_id_inst->id;
}

int32 delete_id(uint32 id) 
{
    // First lookup if an id is already assigned to this id (use id_index_tree)
    id_instance_t* id_inst = avl_lookup(id_index_tree, id);

    if (id_inst == NULL) {
        // Invalid id or no such id allocated yet.
        return ERROR;
    }
    // Id assigned for this user_data
    if ((id_inst->ref_count == 0) || (id_inst->deleted_at != 0)) {
        // this id is already deleted.
        return ERROR;
    }
    // Do some book keeping and mark for garbage collection

    // 1. Update the corresponding chunk allocation_flag
    mark_id_deleted(id_inst->id);
    if (id_inst->ref_count > 0) {
        // non-zero ref-count, no further processing required
        return SUCCESS;
    }

    // 2. remove from data_index_tree & id_index_tree
    data_index_tree = delete_node(data_index_tree, (ulong)id_inst->user_data);
    id_index_tree = delete_node(id_index_tree, (ulong)id);

    // 3. Clear pointer to user-data and set deleted_at for current-time
    id_inst->user_data = NULL; // clear-up

    return SUCCESS;
}

data_ptr query_id(uint32 id) 
{
    // First lookup if an id is already assigned to this id (use id_index_tree)
    id_instance_t* id_inst = (id_instance_t *) avl_lookup(id_index_tree, id);

    // check id_inst is non-null and other verifications
    if ((id_inst == NULL) || (id_inst->ref_count == 0) || (id_inst->deleted_at != 0))
        return NULL;

    return id_inst->user_data;
}

void generate_report(void)
{
    // Iterate over allocated Ids and print on console:
    // there are multiple ways we could do
    // 1. Go through chunk_mgr and filter based on allocation_flag field
    // 2. Inorder traversal of id_index_tree or data_index_tree
}
