#include "order.h"
const char* random_order_type_3() {
    return (rand() % 2 == 0) ? "BUY" : "SELL";
}


void make_abnormal_order_msg_3(char* buffer, size_t bufsize, int client_id) {
    const char* stock_id = "039490";                                         // 키움
    int base_price = 21500;
    int fluctuation = base_price * 0.1;                                      // 10% 변동: 2150원
    int price = base_price - fluctuation + (rand() % (2 * fluctuation + 1)); // 19350 ~ 23650
    //int amount = (rand() % 9 + 1) * 10;                                      // 10 ~ 90
    int amount = 1;                                      // 10 ~ 90
    const char* type = random_order_type_3();                                  // BUY or SELL
    int account_id = 1055;

    snprintf(buffer, bufsize, "%s,%d,%d,%s,%d",
        stock_id, price, amount, type, account_id);
}



DWORD WINAPI client_thread_3(LPVOID arg) {
    int client_id = *(int*)arg;
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char buffer[1024] = { 0 };

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[Client %d] WSAStartup 실패: %d\n", client_id, WSAGetLastError());
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("[Client %d] 소켓 생성 실패: %d\n", client_id, WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    InetPton(AF_INET, L"127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("[Client %d] 서버 연결 실패: %d\n", client_id, WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    srand((unsigned int)time(NULL) + client_id);
    // CSV 포맷으로 직렬화
    char message[256];

    for (int i = 0; i < 60; i++)
    {
        make_abnormal_order_msg_3(message, sizeof(message), client_id);

        send(sock, message, (int)strlen(message), 0);
        printf("[Client %d] 주문 전송: %s\n", client_id, message);
        Sleep(100);
    }
    ;

    int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';
        printf("[Client %d] 서버 응답: %s\n", client_id, buffer);
    }
    else {
        printf("[Client %d] 서버 응답 없음 또는 연결 종료\n", client_id);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
void run_abnormal_order_3() {
    HANDLE threads[CLIENT];
    int client_ids[CLIENT];

    printf("특정 계좌에 대한 비정상 주문 시나리오 시작 >>\n");
    Sleep(2000);

    // 멀티 클라이언트 생성
    for (int i = 0; i < CLIENT; i++) {
        client_ids[i] = i;
        threads[i] = CreateThread(NULL, 0, client_thread_3, &client_ids[i], 0, NULL);
        Sleep(1000);
    }

    // 엔터 키 대기
    printf("엔터 키를 누르면 다음으로 진행합니다...");
    fflush(stdout); // 출력이 버퍼에 남지 않도록
    getchar();      // 첫 번째 엔터 처리용 (만약 이전 입력 버퍼에 엔터가 남아있을 경우)
    getchar();


    WaitForMultipleObjects(CLIENT, threads, TRUE, INFINITE);
    printf("모든 클라이언트 종료\n");
    return;
}

