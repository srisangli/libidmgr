
#include <stdio.h>
#include <defs.h>

#include <id_mgr.h>

void main() {
    printf("Id Mgmt Demo-1\n");

    libid_init(0);

    char* data_1 = "hello";
    uint32 id_1 = create_id(data_1);
    printf("Assigned id=%d for data_1=%s\n", id_1, data_1);
    printf("Assigned id=%d for data_1=%s\n", create_id(data_1), data_1);

    char* data_2 = "world";
    uint32 id_2 = create_id(data_2);
    printf("Assigned id=%d for data_2=%s\n", id_2, data_2);

    printf("Deleting id=%d (of %s) and re-insert\n", id_2, data_2);
    delete_id(id_2);
    id_2 = create_id(data_2);
    printf("Assigned new id=%d for data_2=%s\n", id_2, data_2);
}
