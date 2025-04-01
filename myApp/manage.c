#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "manage.h"
#include "order.h"
#include "transaction.h"
#include "account.h"
#include "fds.h"

volatile int g_stopMonitoring = 0;

void run_manage() {
	int choice;
	while (1) {
		printf("=====================================\n");
		printf("|                                   |\n");
		printf("|          관리자 프로그램          |\n");
		printf("|                                   |\n");
		printf("=====================================\n");
		printf("|          메           뉴          |\n");
		printf("-------------------------------------\n");
		printf("|  1. 고객 조회                     |\n");
		printf("-------------------------------------\n");
		printf("|  2. 거래 내역 관리                |\n");
		printf("-------------------------------------\n");
		printf("|  3. 주문 내역 관리                |\n");
		printf("-------------------------------------\n");
		printf("|  4. 이상 거래 분석 보고서 생성    |\n");
		printf("-------------------------------------\n");
		printf("|  5. 종료                          |\n");
		printf("-------------------------------------\n");
		printf(">> 메뉴를 선택하세요 : ");
		scanf("%d", &choice);
		system("cls");
		switch (choice) {
		case 1: select_all_with_accounts(); to_be_continue(); break;
		case 2: handle_transaction_log(); break;
		case 3: handle_order(); break;
		case 4: report_FDS(); break;
		case 5: return;
		default: printf("올바른 메뉴를 선택하세요\n"); break;
		}

		system("cls");
	}
}

void to_be_continue() {
	char key;
	printf("계속 하시려면 ENTER를 누르세요.. ");
	do {
		key = _getch();
	} while (key != 0x0d);
}

// 실시간 모니터링 함수 (별도 스레드에서 실행)
unsigned __stdcall real_time_monitoring(void* arg) {
	while (!g_stopMonitoring) {
		
		Sleep(500);

		if (_kbhit()) {
			char key = _getch();
			if (key == 'q' || key == 27) {
				g_stopMonitoring = 1;
			}
		}
	}

	return 0;
}
