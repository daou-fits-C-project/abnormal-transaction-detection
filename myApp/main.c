#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "db.h"
#include "socket.h"


int main()
{
    if (db_init() != 0) {
        printf("DB 연결 실패\n");
        return 1;
    }

    setup_server_socket();

    db_cleanup();
    return 0;
}

