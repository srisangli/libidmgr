
#ifndef _id_defn_internal_h_
#define _id_defn_internal_h_

#include <math.h>
#include <defs.h>

typedef struct id_instance_ {
    data_ptr    user_data;
    uint32      ref_count;
    uint64      deleted_at;
    uint32      id;
} id_instance_t;


typedef struct chunk_instance_ {
    uint32                  allocation_flag;
    uint32                  chunk_id;
    struct chunk_instance_  *next_chunk;
    id_instance_t           id_list[0];
} chunk_instance_t;

extern void chunk_mgr_init(void);
extern uint32 epoch_time();
extern id_instance_t* get_free_id_inst();
extern int32 mark_id_deleted(uint32 id);
extern int32 mark_id_assigned(uint32 id);

#endif
