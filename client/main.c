#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h> 

#pragma comment(lib, "ws2_32.lib")
#define PORT 9000
#define CLIENT_COUNT 4
const char* random_order_type() {
    return (rand() % 2 == 0) ? "BUY" : "SELL";
}

void  make_normal_order_msg(char* buffer, size_t bufsize, int client_id) {
    const char* stock_id = "298000";                                         // 다우기술
    int base_price = 21500;
    int fluctuation = base_price * 0.1;                                      // 10% 변동: 2150원
    int price = base_price - fluctuation + (rand() % (2 * fluctuation + 1)); // 19350 ~ 23650
    //int amount = (rand() % 9 + 1) * 10;                                      // 10 ~ 90
    int amount = 1;                                      // 10 ~ 90
    const char* type = random_order_type();                                  // BUY or SELL
    //int account_id = 1000 + client_id;
    int account_id = 1091;

    snprintf(buffer, bufsize, "%s,%d,%d,%s,%d",
        stock_id, price, amount, type, account_id);
}

void  make_abnormal_order_msg_1(char* buffer, size_t bufsize, int client_id) {
    const char* stock_id = "298000";                                         // 다우기술
    int base_price = 21500;
    int fluctuation = base_price * 0.1;                                      // 10% 변동: 2150원
    int price = base_price - fluctuation + (rand() % (2 * fluctuation + 1)); // 19350 ~ 23650
    //int amount = (rand() % 9 + 1) * 10;                                      // 10 ~ 90
    int amount = 1;                                      // 10 ~ 90
    const char* type = random_order_type();                                  // BUY or SELL
    int account_id = 1000 + client_id;
    //int account_id = 1102;

    snprintf(buffer, bufsize, "%s,%d,%d,%s,%d",
        stock_id, price, amount, type, account_id);
}

void make_abnormal_order_msg_2(char* buffer, size_t bufsize, int client_id) {
    const char* stock_id = "023590";                                         // 다우기술
    int base_price = 21500;
    int fluctuation = base_price * 0.1;                                      // 10% 변동: 2150원
    int price = base_price - fluctuation + (rand() % (2 * fluctuation + 1)); // 19350 ~ 23650
    //int amount = (rand() % 9 + 1) * 10;                                      // 10 ~ 90
    int amount = 2000;                                      // 10 ~ 90
    const char* type = random_order_type();                                  // BUY or SELL
    //int account_id = 1000 + client_id;
    int account_id = 1111;

    snprintf(buffer, bufsize, "%s,%d,%d,%s,%d",
        stock_id, price, amount, type, account_id);
}


void make_abnormal_order_msg_3(char* buffer, size_t bufsize, int client_id) {
    const char* stock_id = "039490";                                         // 키움
    int base_price = 21500;
    int fluctuation = base_price * 0.1;                                      // 10% 변동: 2150원
    int price = base_price - fluctuation + (rand() % (2 * fluctuation + 1)); // 19350 ~ 23650
    //int amount = (rand() % 9 + 1) * 10;                                      // 10 ~ 90
    int amount = 1;                                      // 10 ~ 90
    const char* type = random_order_type();                                  // BUY or SELL
    //int account_id = 1000 + client_id;
    int account_id = 1055;

    snprintf(buffer, bufsize, "%s,%d,%d,%s,%d",
        stock_id, price, amount, type, account_id);
}


DWORD WINAPI client_thread(LPVOID arg) {
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
    /*snprintf(message, sizeof(message), "%s,%d,%d,%s,%d",
        stock_id, price, amount, type, account_id);*/
    make_abnormal_order_msg_1(message, sizeof(message), client_id);
    //make_abnormal_order_msg_2(message, sizeof(message), client_id);
    //make_abnormal_order_msg_3(message, sizeof(message), client_id);
    //make_normal_order_msg(message, sizeof(message), client_id);

    send(sock, message, (int)strlen(message), 0);
    printf("[Client %d] 주문 전송: %s", client_id, message);

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

int main() {
    HANDLE threads[CLIENT_COUNT];
    int client_ids[CLIENT_COUNT];

    // 멀티 클라이언트 생성
    for (int i = 0; i < CLIENT_COUNT; i++) {
        client_ids[i] = i;
        threads[i] = CreateThread(NULL, 0, client_thread, &client_ids[i], 0, NULL);
        Sleep(50);  
    }

    WaitForMultipleObjects(CLIENT_COUNT, threads, TRUE, INFINITE);
    printf("모든 클라이언트 종료\n");
    return 0;
   
}
