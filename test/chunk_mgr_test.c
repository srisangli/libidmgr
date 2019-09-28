
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "id_defn_internal.h"
#include "libid_config.h"

void test_free_slot(void)
{
    assert(free_slot(0x0) == 0);
    assert(free_slot(0x8) == 3);
    assert(free_slot(0x20) == 5);
}

extern chunk_instance_t* allocate_chunk(uint32 chunk_id);

void test_allocate_chunk(void)
{
    uint32 i;

    chunk_mgr_init();

    // chunk-id=0
    chunk_instance_t* ch1 = allocate_chunk(0);
    assert(ch1 != NULL);
    assert(ch1->allocation_flag == 0xFFFFFFFF);
    assert(ch1->chunk_id == 0);

    for(i = 0; i < CHUNK_SIZE; i++)
    {
        assert(ch1->id_list[i].id == i+1);
    }

    // chunk-id=4
    chunk_instance_t* ch4 = allocate_chunk(4);
    assert(ch4 != NULL);
    assert(ch4->allocation_flag == 0xFFFFFFFF);
    assert(ch4->chunk_id == 4);

    uint32 base = CHUNK_SIZE * 4;
    for(i = 0; i < CHUNK_SIZE; i++)
    {
        assert(ch4->id_list[i].id == base+i+1);
    }
}

void test_check_free_id_inst_internal(void)
{
    uint32 i;
    chunk_mgr_init();

    // chunk-id=2
    chunk_instance_t* ch2 = allocate_chunk(2);

    uint32 free_id = check_free_id_count_internal(ch2);
    assert(free_id == CHUNK_SIZE); // all are available

    ch2->allocation_flag = 0x1F;
    assert(check_free_id_count_internal(ch2) == CHUNK_SIZE); // all GCed

    for(i = 0; i < 5; i++)
    {
        ch2->id_list[i].ref_count == i + 1;
    }
    assert(check_free_id_count_internal(ch2) == 27); // 5 cannot be GCed

    uint32 current = epoch_time();
    for(i = 0; i < 5; i++)
    {
        ch2->id_list[i].ref_count = 0;
        ch2->id_list[i].deleted_at == epoch_time;
    }
    assert(check_free_id_count_internal(ch2) == 27); // 5 cannot be GCed

    sleep(GC_TIMEOUT + 10); // sleep for GC-timeout
    assert(check_free_id_count_internal(ch2) == 32); // 5 GCed!!
}

void main()
{
    test_free_slot();
    test_allocate_chunk();
    test_check_free_id_inst_internal();
}
