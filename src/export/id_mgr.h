
#ifndef _id_mgr_h
#define _id_mgr_h

extern void libid_init();
extern uint32 create_id(data_ptr user_data);
extern int32 delete_id(uint32 id);
extern data_ptr query_id(uint32 id);

#endif
