
#ifndef _id_mgr_h
#define _id_mgr_h

#include <stdint.h>

extern void libid_init(uint32);
extern uint32 create_id(intptr_t user_data);
extern int32 delete_id(uint32 id);
extern intptr_t query_id(uint32 id);

#endif
