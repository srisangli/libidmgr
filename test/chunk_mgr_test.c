
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "id_defn_internal.h"
#include "libid_config.h"

#define UT_GC_TIMEOUT   5

void test_free_slot(void)
{
    printf("%s: ", __func__);
    assert(free_slot(0x0) == 0);
    assert(free_slot(0x8) == 3);
    assert(free_slot(0x20) == 5);
    printf(" PASSED\n");
}

extern chunk_instance_t* allocate_chunk(uint32 chunk_id);
extern chunk_instance_t* get_free_id_chunk_internal(chunk_instance_t *);
extern chunk_instance_t* get_free_id_chunk(void);

void test_allocate_chunk(void)
{
    uint32 i;

    printf("%s: ", __func__);

    chunk_mgr_init(UT_GC_TIMEOUT);

    // chunk-id=0
    chunk_instance_t* ch1 = allocate_chunk(0);
    assert(ch1 != NULL);
    assert(ch1->avail_mask == 0xFFFFFFFF);
    assert(ch1->chunk_id == 0);

    for(i = 0; i < CHUNK_SIZE; i++)
    {
        assert(ch1->id_list[i].id == i+1);
    }

    // chunk-id=4
    chunk_instance_t* ch4 = allocate_chunk(4);
    assert(ch4 != NULL);
    assert(ch4->avail_mask == 0xFFFFFFFF);
    assert(ch4->chunk_id == 4);

    uint32 base = CHUNK_SIZE * 4;
    for(i = 0; i < CHUNK_SIZE; i++)
    {
        assert(ch4->id_list[i].id == base+i+1);
    }
    printf(" PASSED\n");
}

void test_check_free_id_inst_internal(void)
{
    uint32 i;

    printf("%s: ", __func__);
    chunk_mgr_init(UT_GC_TIMEOUT);

    // chunk-id=2
    chunk_instance_t* ch2 = allocate_chunk(2);

    uint32 free_id = check_free_id_count_internal(ch2);
    assert(free_id == CHUNK_SIZE); // all are available

    ch2->avail_mask = 0xFFFFFFE0;
    for(i = 0; i < 5; i++)
    {
        ch2->id_list[i].ref_count = i + 1;
    }
    assert(check_free_id_count_internal(ch2) == 27);

    uint32 current = epoch_time();
    for(i = 0; i < 5; i++)
    {
        ch2->id_list[i].deleted_at = current;
        ch2->id_list[i].ref_count -= (i + 1);
    }
    ch2->gc_mask = 0x1F;
    assert(check_free_id_count_internal(ch2) == 27); // 5 cannot be GCed

    sleep(UT_GC_TIMEOUT + 1); // sleep for GC-timeout
    assert(check_free_id_count_internal(ch2) == 32); // 5 GCed!!
    printf(" PASSED\n");
}

void test_get_free_id_chunk_internal(void) 
{
    printf("%s: ", __func__);

    chunk_mgr_init(UT_GC_TIMEOUT);
    chunk_instance_t* ch3 = allocate_chunk(3);
    // mark all as assined
    ch3->avail_mask = 0x0;
    chunk_instance_t* ch4 = get_free_id_chunk_internal(ch3);
    assert(ch4->chunk_id == 4);

    // Mark all as assigned in new chunk
    ch4->avail_mask = 0x0;
    chunk_instance_t* ch5 = get_free_id_chunk_internal(ch3);
    assert(ch5->chunk_id == 5);

    // Free up few items in ch4
    ch4->avail_mask = 0x1000;
    chunk_instance_t* ch_t = get_free_id_chunk_internal(ch3);
    assert(ch_t->chunk_id == 4);

    printf(" PASSED\n");
}

void test_get_free_id_chunk(void)
{
    printf("%s: ", __func__);

    chunk_mgr_init(UT_GC_TIMEOUT);
    chunk_instance_t* ch0 = get_free_id_chunk();
    assert(ch0->chunk_id == 0);
    ch0->avail_mask = 0x0; // mark all as allocated

    chunk_instance_t* ch1 = get_free_id_chunk();
    assert(ch1->chunk_id == 1);

    printf(" PASSED\n");
}

void test_mark_id_assigned(void)
{
    uint32 pos;

    printf("%s: ", __func__);
    chunk_mgr_init(UT_GC_TIMEOUT);
    chunk_instance_t* ch0 = get_free_id_chunk();

    for(pos = 0; pos < CHUNK_SIZE; pos++)
    {
        mark_id_assigned(ch0->id_list[pos].id);
    }

    chunk_instance_t* ch1 = get_free_id_chunk();
    for(pos = 0; pos < CHUNK_SIZE; pos++)
    {
        mark_id_assigned(ch1->id_list[pos].id);
    }
    chunk_instance_t* ch2 = get_free_id_chunk();

    for(pos = 0; pos < CHUNK_SIZE/2; pos++)
    {
        mark_id_assigned(ch1->id_list[pos * 2].id);
    }
    assert(ch2->chunk_id == 2);
    assert(ch0->avail_mask == 0x0);
    assert(ch1->avail_mask == 0x0);

    for(pos = 0; pos < CHUNK_SIZE/2; pos++)
    {
        mark_id_assigned(ch2->id_list[pos * 2 + 1].id);
    }
    chunk_instance_t* ch_t = get_free_id_chunk();
    assert(ch_t->chunk_id == 2);

    printf(" PASSED\n");
}

void test_mark_id_deleted(void)
{
    uint32 pos;

    printf("%s: ", __func__);
    chunk_mgr_init(UT_GC_TIMEOUT);
    chunk_instance_t* ch0 = get_free_id_chunk();

    for(pos = 0; pos < CHUNK_SIZE; pos++)
    {
        mark_id_assigned(ch0->id_list[pos].id);
    }

    chunk_instance_t* ch1 = get_free_id_chunk();
    for(pos = 0; pos < CHUNK_SIZE; pos++)
    {
        mark_id_assigned(ch1->id_list[pos].id);
    }
    chunk_instance_t* ch2 = get_free_id_chunk();

    mark_id_deleted(ch1->id_list[8].id);
    chunk_instance_t* ch_t1 = get_free_id_chunk();
    assert(ch_t1->chunk_id == 2);
    assert(ch0->avail_mask == 0x0);
    assert(ch1->avail_mask == 0x0);
    assert(ch1->gc_mask == 0x100);
    assert(ch2->avail_mask == 0xFFFFFFFF);

    sleep(UT_GC_TIMEOUT + 1);
    chunk_instance_t* ch_t2 = get_free_id_chunk();
    assert(ch_t2->chunk_id == 1);

    for(pos = 0; pos < CHUNK_SIZE/2; pos++)
    {
        mark_id_deleted(ch0->id_list[pos * 2].id);
    }
    mark_id_assigned(ch1->id_list[8].id);
    chunk_instance_t* ch_t3 = get_free_id_chunk();

    assert(ch0->avail_mask == 0x0);
    assert(ch0->gc_mask == 0x55555555);
    assert(ch1->avail_mask == 0x0);
    assert(ch2->avail_mask == 0xFFFFFFFF);
    assert(ch_t3->chunk_id == 2);

    printf(" PASSED\n");
}

void main()
{
    test_free_slot();
    test_allocate_chunk();
    test_check_free_id_inst_internal(); // has sleep for GC timeout
    test_get_free_id_chunk_internal();
    test_get_free_id_chunk();
    test_mark_id_assigned();
    test_mark_id_deleted(); // has sleep for GC timeout
}
