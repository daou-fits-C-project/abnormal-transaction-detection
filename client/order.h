#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h> 

#pragma comment(lib, "ws2_32.lib")
#define PORT 9000
#define MULTI_CLIENT 10
#define CLIENT 1


void run_normal_order();
void run_abnormal_order_1();
void run_abnormal_order_2();
void run_abnormal_order_3();