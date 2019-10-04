
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <defs.h>

#include "libid_config.h"
#include "id_defn_internal.h"

chunk_instance_t    *chunk_list_head;
uint32              gc_timeout;

// Forward declarations
uint32 free_slot(uint32 allocation_flag);
chunk_instance_t* get_free_id_chunk(void);
uint32 check_free_id_count_internal(chunk_instance_t* ch_inst);
chunk_instance_t* get_free_id_chunk_internal(chunk_instance_t* ch_inst);

//
// Library Initialization routine
//
void chunk_mgr_init(uint32 gc_time) 
{
    chunk_list_head = NULL;
    gc_timeout = gc_time;
}

uint32 epoch_time()
{
    time_t seconds;

    seconds = time(NULL);
    return seconds;
}

//
// API to get new id_instance_t for new allocation
//
id_instance_t* get_free_id_inst()
{
    // Look for first available chunk with available-slots
    chunk_instance_t* ch_inst = get_free_id_chunk();

    uint32 free_slot_id = free_slot(ch_inst->avail_mask) % CHUNK_SIZE;
    return &(ch_inst->id_list[free_slot_id]);
}

//
// Return 0-based right-most bit which is set to 1
//
uint32 free_slot(uint32 allocation_flag)
{
    return log2(allocation_flag & -allocation_flag);
}

//
// Return number of bits set in a uint32
//
uint32 bit_count(uint32 mask)
{
    uint32 count = 0;
    while (mask)
    {
        mask &= mask - 1;
        count++;
    }
    return count;
}

//
// Internal API to allocate a new chunk
//
chunk_instance_t* allocate_chunk(uint32 chunk_id)
{
    uint32 pos;
    uint32 malloc_size = sizeof(chunk_instance_t) + sizeof(id_instance_t) * CHUNK_SIZE;
    chunk_instance_t* ch = (chunk_instance_t *) calloc(malloc_size, 1);
    ch->avail_mask = 0xFFFFFFFF;
    ch->chunk_id = chunk_id;
    for (pos = 0; pos < CHUNK_SIZE; pos++)
    {
        ch->id_list[pos].id = (chunk_id * CHUNK_SIZE) + pos + 1;
    }

    return ch;
}

//
// Internal API to look for chunk with free-id
//
chunk_instance_t* get_free_id_chunk(void) 
{
    if (chunk_list_head == NULL) 
    {
        // this is the first instace of chunk, create with id=0
        chunk_list_head = allocate_chunk(0);
    }

    return get_free_id_chunk_internal(chunk_list_head);
}

//
// Internal API to recursively look for chunk_instance with free id
//
chunk_instance_t* get_free_id_chunk_internal(chunk_instance_t* ch_inst) 
{
    uint32 free_count = 0;
    if ((ch_inst->avail_mask != 0x0) || (ch_inst->gc_mask != 0x0))
    {
        // There are few spots, verify and do GC if possible
        free_count = check_free_id_count_internal(ch_inst);
    }
    
    if (free_count > 0) 
        return ch_inst;

    if (ch_inst->next_chunk == NULL)
    {
        // this was the last chunk in the list
        ch_inst->next_chunk = allocate_chunk(ch_inst->chunk_id + 1);
    }
    return get_free_id_chunk_internal(ch_inst->next_chunk);
}

//
// Internal API to check if any id is free and do garbage collection
//
uint32 check_free_id_count_internal(chunk_instance_t* ch_inst)
{
    uint32 pos;
    if ((ch_inst->avail_mask == 0x0) && (ch_inst->gc_mask == 0x0))
    {
        // everything is allocated and nothing to GC
        return 0;
    }

    uint32 free_id_count = bit_count(ch_inst->avail_mask);

    // For now, iterate over CHUNK_SIZE entries to see if anything 
    // can be garbage-collected
    ulong current_epoch = epoch_time();
    for (pos = 0; ch_inst->gc_mask && pos < CHUNK_SIZE; pos++)
    {
        if ((ch_inst->gc_mask & (1 << pos)) == 0x0)
            continue; // GC-bit not set

        // Before using this id_inst, verify once.
        assert(ch_inst->id_list[pos].ref_count == 0);

        if (current_epoch - ch_inst->id_list[pos].deleted_at > gc_timeout) 
        {
            // do garbage-collection
            ch_inst->id_list[pos].deleted_at = 0x0;
            free_id_count++;
            // mark this as available and do reset gc-mask
            ch_inst->avail_mask |= (1 << pos);
            ch_inst->gc_mask &= ~(1 << pos);
        }
    }
    return free_id_count;
}

//
// API to update chunk allocation-flag when id is marked for deletion
//
int32 mark_id_deleted(uint32 id)
{
    uint32 chunk_id = (id - 1)/CHUNK_SIZE;

    chunk_instance_t* ch_inst = chunk_list_head;
    while ((ch_inst != NULL) && (ch_inst->chunk_id != chunk_id))
    {
        ch_inst = ch_inst->next_chunk;
    }

    if (ch_inst == NULL) 
    {
        return ERROR; // error condition
    }
    uint32 pos = (id - 1) % CHUNK_SIZE;
    if (ch_inst->id_list[pos].ref_count > 0) {
        ch_inst->id_list[pos].ref_count--;
        if (ch_inst->id_list[pos].ref_count == 0) // not referenced anymore
        {
            ch_inst->id_list[pos].deleted_at = epoch_time();
            ch_inst->gc_mask |= 1 << pos;
        }
    }
    return SUCCESS;
}

//
// API to update chunk allocation-flag when id is assigned
//
int32 mark_id_assigned(uint32 id)
{
    uint32 chunk_id = (id - 1)/CHUNK_SIZE;

    chunk_instance_t* ch_inst = chunk_list_head;
    while ((ch_inst != NULL) && (ch_inst->chunk_id != chunk_id))
    {
        ch_inst = ch_inst->next_chunk;
    }

    if (ch_inst == NULL) 
    {
        return ERROR; // error condition
    }
    uint32 pos = (id - 1) % CHUNK_SIZE;
    ch_inst->avail_mask &= ~(1 << pos);
    ch_inst->id_list[pos].ref_count++;
    ch_inst->id_list[pos].deleted_at = 0;
    return SUCCESS;
}
