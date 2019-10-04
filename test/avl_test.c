
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "avl.h"

void test_tree_insert_delete_query(void)
{
    avl_node_t* root = NULL;

    printf("%s: ", __func__);
    char* x = "hello";
    char* y = "world";
    assert(avl_lookup(root, 10) == (intptr_t)NULL);
    assert(delete_node(root, 4) == NULL);

    root = insert_node(root, (intptr_t)10, (intptr_t)x);
    root = insert_node(root, (intptr_t)4, (intptr_t)y);

    char* dt = (char *)avl_lookup(root, (intptr_t)10);
    assert(strcmp(dt, "hello") == 0);

    dt = (char *)avl_lookup(root, 4);
    assert(strcmp(dt, "world") == 0);
    root = delete_node(root, (intptr_t)10);
    assert(avl_lookup(root, (intptr_t)10) == (intptr_t)NULL);
    printf(" PASSED\n");
}

void test_tree_delete_test(void)
{
    avl_node_t* root = NULL;

    printf("%s: ", __func__);
    root = insert_node(root, (intptr_t)10, (intptr_t)"hello");
    root = insert_node(root, (intptr_t)4, (intptr_t)"world");
    root = insert_node(root, (intptr_t)3, (intptr_t)"check");
    root = insert_node(root, (intptr_t)13, (intptr_t)"verify");
    root = insert_node(root, (intptr_t)23, (intptr_t)"validate");
    root = insert_node(root, (intptr_t)29, (intptr_t)"review");
    root = insert_node(root, (intptr_t)19, (intptr_t)"follow");
    root = insert_node(root, (intptr_t)10, (intptr_t)"overwrite");

    char* dt = (char *)avl_lookup(root, (intptr_t)10);
    assert(strcmp(dt, "overwrite") == 0);

    dt = (char *)avl_lookup(root, (intptr_t)4);
    assert(strcmp(dt, "world") == 0);

    root = delete_node(root, (intptr_t)10);
    assert(avl_lookup(root, (intptr_t)10) == (intptr_t)NULL);
    root = insert_node(root, (intptr_t)10, (intptr_t)"revert");

    root = delete_node(root, (intptr_t)19);
    assert(avl_lookup(root, (intptr_t)19) == (intptr_t)NULL);

    root = delete_node(root, (intptr_t)29);
    assert(avl_lookup(root, (intptr_t)29) == (intptr_t)NULL);

    root = delete_node(root, (intptr_t)23);
    assert(avl_lookup(root, (intptr_t)23) == (intptr_t)NULL);

    root = delete_node(root, (intptr_t)19);
    assert(avl_lookup(root, (intptr_t)19) == (intptr_t)NULL);

    dt = (char *)avl_lookup(root, (intptr_t)13);
    assert(strcmp(dt, "verify") == 0);

    dt = (char *)avl_lookup(root, (intptr_t)10);
    assert(strcmp(dt, "revert") == 0);
    printf(" PASSED\n");
}

// TODO: test-cases for ht verification, balancing

void main()
{
    test_tree_insert_delete_query();
    test_tree_delete_test();
}
