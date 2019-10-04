
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include <defs.h>
#include <id_mgr.h>

#define UT_GC_TIMEOUT   5

void test_create_query(void)
{
    char* dt1 = "tree";
    
    printf("%s: ", __func__);
    libid_init(UT_GC_TIMEOUT);

    uint32 dt1_id = create_id((intptr_t) dt1);
    assert(dt1_id == 1);

    char* dt1_t = (char *)query_id(dt1_id);
    assert(strcmp(dt1_t, dt1) == 0);
    printf(" PASSED\n");
}

void test_create_delete_query(void)
{
    char* dt1 = "another tree";
    char* dt2 = "first tree";
    char* dt3 = "green tree";
    printf("%s: ", __func__);
    
    libid_init(UT_GC_TIMEOUT);

    uint32 dt1_id = create_id((intptr_t) dt1);
    assert(dt1_id == 1);

    uint32 dt1_id_t = create_id((intptr_t) dt1);
    assert(dt1_id == dt1_id_t);

    char* dt1_t = (char *)query_id(dt1_id);
    assert(strcmp(dt1_t, dt1) == 0);

    uint32 dt2_id = create_id((intptr_t) dt2);
    assert(dt2_id == 2);

    uint32 dt3_id = create_id((intptr_t) dt3);
    assert(dt3_id == 3);

    delete_id(dt1_id);
    delete_id(dt2_id);
    delete_id(dt3_id);

    dt1_t = (char *)query_id(dt1_id);
    assert(strcmp(dt1_t, dt1) == 0);

    char* dt2_t = (char *)query_id(dt2_id);
    assert(dt2_t == NULL);

    char* dt3_t = (char *)query_id(dt3_id);
    assert(dt3_t == NULL);
    printf(" PASSED\n");
}

void test_create_delete_create(void)
{
    char* dt1 = "blue tree";
    char* dt2 = "green tree";
    char* dt3 = "yellow tree";
    char* dt4 = "red tree";
    
    printf("%s: ", __func__);
    libid_init(UT_GC_TIMEOUT);

    uint32 dt1_id = create_id((intptr_t) dt1);
    uint32 dt2_id = create_id((intptr_t) dt2);
    uint32 dt3_id = create_id((intptr_t) dt3);
    uint32 dt4_id = create_id((intptr_t) dt4);

    delete_id(dt1_id);
    delete_id(dt2_id);
    delete_id(dt3_id);
    delete_id(dt4_id);

    uint32 dt1_id_n = create_id((intptr_t) dt1);
    uint32 dt2_id_n = create_id((intptr_t) dt2);
    uint32 dt3_id_n = create_id((intptr_t) dt3);
    uint32 dt4_id_n = create_id((intptr_t) dt4);
    assert(dt1_id != dt1_id_n);
    assert(dt2_id != dt2_id_n);
    assert(dt3_id != dt3_id_n);
    assert(dt4_id != dt4_id_n);
    printf(" PASSED\n");
}

void test_create_delete_gc_create(void)
{
    char* dt1 = "blue tree";
    char* dt2 = "green tree";
    char* dt3 = "yellow tree";
    char* dt4 = "red tree";
    
    printf("%s: ", __func__);
    libid_init(UT_GC_TIMEOUT);

    uint32 dt1_id = create_id((intptr_t) dt1);
    uint32 dt2_id = create_id((intptr_t) dt2);
    uint32 dt3_id = create_id((intptr_t) dt3);
    uint32 dt4_id = create_id((intptr_t) dt4);

    delete_id(dt1_id);
    delete_id(dt2_id);
    delete_id(dt3_id);
    delete_id(dt4_id);

    sleep(UT_GC_TIMEOUT + 1);

    uint32 dt1_id_n = create_id((intptr_t) dt1);
    uint32 dt2_id_n = create_id((intptr_t) dt2);
    uint32 dt3_id_n = create_id((intptr_t) dt3);
    uint32 dt4_id_n = create_id((intptr_t) dt4);
    assert(dt1_id == dt1_id_n);
    assert(dt2_id == dt2_id_n);
    assert(dt3_id == dt3_id_n);
    assert(dt4_id == dt4_id_n);
    printf(" PASSED\n");
}

void test_create_ref_count_delete_gc_create(void)
{
    char* dt1 = "blue tree";
    char* dt2 = "green tree";
    char* dt3 = "yellow tree";
    char* dt4 = "red tree";
    
    printf("%s: ", __func__);
    libid_init(UT_GC_TIMEOUT);

    // ref-count=1
    uint32 dt1_id = create_id((intptr_t) dt1);
    uint32 dt2_id = create_id((intptr_t) dt2);
    uint32 dt3_id = create_id((intptr_t) dt3);
    uint32 dt4_id = create_id((intptr_t) dt4);

    // ref-count=2
    assert(dt1_id == create_id((intptr_t) dt1));
    assert(dt2_id == create_id((intptr_t) dt2));

    // ref-count=3
    assert(dt1_id == create_id((intptr_t) dt1));
    assert(dt2_id == create_id((intptr_t) dt2));

    // ref-count=4
    assert(dt1_id == create_id((intptr_t) dt1));
    assert(dt2_id == create_id((intptr_t) dt2));

    delete_id(dt1_id); // ref-count=3
    delete_id(dt2_id); // ref-count=3
    delete_id(dt3_id); // ref-count=0
    delete_id(dt4_id); // ref-count=0

    delete_id(dt1_id); // ref-count=2
    delete_id(dt2_id); // ref-count=2

    delete_id(dt1_id); // ref-count=1
    delete_id(dt2_id); // ref-count=1

    delete_id(dt2_id); // ref-count=0
    sleep(UT_GC_TIMEOUT + 1);

    uint32 dt1_id_n = create_id((intptr_t) dt1); // ref-count=2
    uint32 dt2_id_n = create_id((intptr_t) dt2); // ref-count=1 (re-use)
    assert(dt1_id == dt1_id_n);
    assert(dt2_id == dt2_id_n);
    printf(" PASSED\n");
}

void main(void)
{
    test_create_query();
    test_create_delete_query();
    test_create_delete_create();
    test_create_delete_gc_create();
    test_create_ref_count_delete_gc_create();
}
