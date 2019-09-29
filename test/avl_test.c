
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
    assert(avl_lookup(root, 10) == NULL);
    assert(delete_node(root, 4) == NULL);

    root = insert_node(root, 10, x);
    root = insert_node(root, 4, y);

    char* dt = (char *)avl_lookup(root, 10);
    assert(strcmp(dt, "hello") == 0);

    dt = (char *)avl_lookup(root, 4);
    assert(strcmp(dt, "world") == 0);
    root = delete_node(root, 10);
    assert(avl_lookup(root, 10) == NULL);
    printf(" PASSED\n");
}

void test_tree_delete_test(void)
{
    avl_node_t* root = NULL;

    printf("%s: ", __func__);
    root = insert_node(root, 10, "hello");
    root = insert_node(root, 4, "world");
    root = insert_node(root, 3, "check");
    root = insert_node(root, 13, "verify");
    root = insert_node(root, 23, "validate");
    root = insert_node(root, 29, "review");
    root = insert_node(root, 19, "follow");
    root = insert_node(root, 10, "overwrite");

    char* dt = (char *)avl_lookup(root, 10);
    assert(strcmp(dt, "overwrite") == 0);

    dt = (char *)avl_lookup(root, 4);
    assert(strcmp(dt, "world") == 0);

    root = delete_node(root, 10);
    assert(avl_lookup(root, 10) == NULL);
    root = insert_node(root, 10, "revert");

    root = delete_node(root, 19);
    assert(avl_lookup(root, 19) == NULL);

    root = delete_node(root, 29);
    assert(avl_lookup(root, 29) == NULL);

    root = delete_node(root, 23);
    assert(avl_lookup(root, 23) == NULL);

    root = delete_node(root, 19);
    assert(avl_lookup(root, 19) == NULL);

    dt = (char *)avl_lookup(root, 13);
    assert(strcmp(dt, "verify") == 0);

    dt = (char *)avl_lookup(root, 10);
    assert(strcmp(dt, "revert") == 0);
    printf(" PASSED\n");
}

// TODO: test-cases for ht verification, balancing

void main()
{
    test_tree_insert_delete_query();
    test_tree_delete_test();
}
