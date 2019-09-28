
#include <stdio.h>
#include <defs.h>

#include <id_mgr.h>

void main() {
    printf("Id Mgmt Demo-1\n");

    libid_init();

    char* data_1 = "hello";
    uint32 id_1 = create_id(data_1);
    printf("Assigned id=%d for data_1=%s\n", id_1, data_1);
    printf("Assigned id=%d for data_1=%s\n", create_id(data_1), data_1);

    char* data_2 = "world";
    uint32 id_2 = create_id(data_1);
    printf("Assigned id=%d for data_2=%s\n", id_2, data_2);
}
