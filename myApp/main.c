#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "db.h"
#include "socket.h"
#include "account.h"


int main()
{
    if (db_init() != 0) {
        fprintf(stderr, "DB 연결 실패\n");
        return 1;
    }

    // 소켓 서버 시작(스레드 분리)
    SOCKET server_sock = setup_server_socket();
    if (server_sock == INVALID_SOCKET) {
        fprintf(stderr, "서버 소켓 초기화 실패\n");
        db_cleanup();
        return 1;
    }

    // 여기에 main 로직 들어갈 수 있음
    //handle_account();


    // 소켓 종료 및 자원 해제
    cleanup_server_socket(&server_sock);


    db_cleanup();

    return 0;
}

