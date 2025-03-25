#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "db.h"


int main()
{
    if (db_init() != 0) {
        printf("DB 연결 실패\n");
        return 1;
    }
    db_cleanup();
    return 0;
}

