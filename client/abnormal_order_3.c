#include "order.h"
const char* random_order_type_3() {
    return (rand() % 2 == 0) ? "BUY" : "SELL";
}


void make_abnormal_order_msg_3(char* buffer, size_t bufsize, int client_id) {
    const char* stock_id = "039490";                                         // Ű��
    int base_price = 21500;
    int fluctuation = base_price * 0.1;                                      // 10% ����: 2150��
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
        printf("[Client %d] WSAStartup ����: %d\n", client_id, WSAGetLastError());
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("[Client %d] ���� ���� ����: %d\n", client_id, WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    InetPton(AF_INET, L"127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("[Client %d] ���� ���� ����: %d\n", client_id, WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    srand((unsigned int)time(NULL) + client_id);
    // CSV �������� ����ȭ
    char message[256];

    for (int i = 0; i < 60; i++)
    {
        make_abnormal_order_msg_3(message, sizeof(message), client_id);

        send(sock, message, (int)strlen(message), 0);
        printf("[Client %d] �ֹ� ����: %s\n", client_id, message);
        Sleep(100);
    }
    ;

    int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';
        printf("[Client %d] ���� ����: %s\n", client_id, buffer);
    }
    else {
        printf("[Client %d] ���� ���� ���� �Ǵ� ���� ����\n", client_id);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
void run_abnormal_order_3() {
    HANDLE threads[CLIENT];
    int client_ids[CLIENT];

    printf("Ư�� ���¿� ���� ������ �ֹ� �ó����� ���� >>\n");
    Sleep(2000);

    // ��Ƽ Ŭ���̾�Ʈ ����
    for (int i = 0; i < CLIENT; i++) {
        client_ids[i] = i;
        threads[i] = CreateThread(NULL, 0, client_thread_3, &client_ids[i], 0, NULL);
        Sleep(1000);
    }

    // ���� Ű ���
    printf("���� Ű�� ������ �������� �����մϴ�...");
    fflush(stdout); // ����� ���ۿ� ���� �ʵ���
    getchar();      // ù ��° ���� ó���� (���� ���� �Է� ���ۿ� ���Ͱ� �������� ���)
    getchar();


    WaitForMultipleObjects(CLIENT, threads, TRUE, INFINITE);
    printf("��� Ŭ���̾�Ʈ ����\n");
    return;
}

