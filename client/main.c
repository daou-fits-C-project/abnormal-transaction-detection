#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h> 

#pragma comment(lib, "ws2_32.lib")
#define PORT 9000
#define CLIENT_COUNT 10


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

    // 주문 메시시 담기 (for 루프를 통해 반복 요청도 가능)
    char message[128];
    sprintf(message, "ORDER: BUY %d\n", client_id * 10); 
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
        client_ids[i] = i + 1;
        threads[i] = CreateThread(NULL, 0, client_thread, &client_ids[i], 0, NULL);
        Sleep(50);  // 연결 간 약간의 시간차 (너무 몰리면 서버가 accept 처리 못함)
    }

    WaitForMultipleObjects(CLIENT_COUNT, threads, TRUE, INFINITE);
    printf("모든 클라이언트 종료\n");
    return 0;
   
}
