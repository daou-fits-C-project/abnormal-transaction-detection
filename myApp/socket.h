#pragma once
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>   // Visual Studio에서 필요!
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_THREADS 4
#define PORT 9000

int setup_server_socket();
DWORD WINAPI receiver_thread(LPVOID lpParam);
DWORD WINAPI worker_thread(LPVOID lpParam);