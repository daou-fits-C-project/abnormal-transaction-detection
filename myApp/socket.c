#define _CRT_SECURE_NO_WARNINGS

#include "socket.h"
#include "order_queue.h"

#pragma comment(lib, "ws2_32.lib") // 링커 설정

int setup_server_socket() {
    // 1. WinSock 초기화 (Windows에서만 필요)
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup 실패: %d\n", WSAGetLastError());
        return 1;
    }

    // 2. 소켓 생성
    SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        printf("소켓 생성 실패: %d\n", WSAGetLastError());
        return 1;
    }

    // 3. 주소 정보 설정
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스 허용

    // 4. bind
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("bind 실패: %d\n", WSAGetLastError());
        return 1;
    }

    // 5. listen
    listen(server_sock, SOMAXCONN);
    printf("서버 대기 중 ... (포트: %d) \n", PORT);


    // 6. critical section 초기화
    InitializeCriticalSection(&queue_lock);
    queue_not_empty_event = CreateEvent(NULL, FALSE, FALSE, NULL);

    // 7. 수신/워커 thread 분리
    CreateThread(NULL, 0, receiver_thread, &server_sock, 0, NULL);
    for (int i = 0; i < MAX_THREADS; i++)
    {
        CreateThread(NULL, 0, worker_thread, NULL, 0, NULL);

    }
    while (1) Sleep(1000);

    // 8. 종료 처리
    closesocket(server_sock);
    WSACleanup();
    return 0;
}


DWORD WINAPI receiver_thread(LPVOID lpParam) {
    OrderTask task;
    SOCKET server = *(SOCKET*)lpParam;
    fd_set master_set;
    FD_ZERO(&master_set);
    FD_SET(server, &master_set);

    while (1) {
        fd_set read_fds = master_set;

        select(0, &read_fds, NULL, NULL, NULL);
        int result = select(0, &read_fds, NULL, NULL, NULL);
        if (result == SOCKET_ERROR) {
            printf("select() 오류: %d\n", WSAGetLastError());
            continue;
        }
        for (u_int i = 0; i < read_fds.fd_count; i++) {
                SOCKET s = read_fds.fd_array[i];

                if (s == server) {
                    SOCKET client = accept(server, NULL, NULL);
                    FD_SET(client, &master_set);
                    printf("클라이언트 접속: %d\n", (int)client);
                }
                else {
                    char buf[1024] = { 0 };
                    int len = recv(s, buf, sizeof(buf), 0);
                    printf("소켓 % d → recv() 결과: % d 바이트\n", s, len);
                    if (len <= 0) {
                        closesocket(s);
                        FD_CLR(s, &master_set);
                        printf("클라이언트 종료: %d\n", s);
                    }
                    else {
                        task.client_socket = s;
                        strncpy(task.data, buf, sizeof(task.data));
                        printf("[Receiver] client: %d, data: %s\n", task.client_socket, task.data);
                        enqueue(task);
                    }
                }
            }
  
    }
    return 0;
}


DWORD WINAPI worker_thread(LPVOID lpParam) {
    while (1) {
        WaitForSingleObject(queue_not_empty_event, INFINITE);
        OrderTask task = dequeue();

        // 주문 처리 로직 함수 
        printf("[Worker] 주문 처리: %s (소켓: %d)\n", task.data, (int)task.client_socket);
        const char* response = "ORDER RECEIVED\n";
        send(task.client_socket, response, (int)strlen(response), 0);
    }
    return 0;
}