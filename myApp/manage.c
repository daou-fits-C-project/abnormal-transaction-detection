#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "manage.h"
#include "order.h"
#include "transaction.h"
#include "account.h"

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
		printf("|  4. 이상 거래 분석 보고서         |\n");
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
		case 4: abnormal_analysis_report(); break;
		case 5: return;
		default: printf("올바른 메뉴를 선택하세요\n"); break;
		}

		system("cls");
	}
}

void handle_transaction_log() {
	int choice, account_id, status;
	char stock_id[STOCK_ID_BUF];
	system("cls");
	while (1) {
		printf("=====================================\n");
		printf("|                                   |\n");
		printf("|      거래 내역 관리 프로그램      |\n");
		printf("|                                   |\n");
		printf("=====================================\n");
		printf("|          메           뉴          |\n");
		printf("-------------------------------------\n");
		printf("|  1. 계좌 별 거래 내역 조회        |\n");
		printf("-------------------------------------\n");
		printf("|  2. 종목 별 거래 내역 조회        |\n");
		printf("-------------------------------------\n");
		printf("|  3. 계좌 상태 관리                |\n");
		printf("-------------------------------------\n");
		printf("|  4. 관리자 프로그램으로 돌아가기  |\n");
		printf("-------------------------------------\n");
		printf(">> 메뉴를 선택하세요 : ");
		scanf("%d", &choice);
		system("cls");
		switch (choice) {
		case 1: 
			printf(">> 찾고자 하는 거래 계좌 ID : ");
			scanf("%d", &account_id);
			select_account_transaction(account_id); to_be_continue(); break;
		case 2: 
			printf(">> 찾고자 하는 거래 종목 코드 : ");
			scanf("%s", stock_id);
			select_stock_transaction(stock_id); to_be_continue(); break;
		case 3: 
			printf(">> 수정할 계좌 ID: ");
			scanf("%d", &account_id);
			printf(">> 상태 입력 (0: 정상, 1: 일시 정지, 2: 잠김, 3 :해지됨, 4: 개설 중, 5: 동결됨, 6: 인증 미완료, 7: 제한 계좌 ");
			scanf("%d", &status);
			update_account_status(account_id, status);
			handle_order(); break;
		case 4: return;
		default: printf("올바른 메뉴를 선택하세요\n"); break;
		}
		system("cls");
	}
}

void abnormal_analysis_report() {

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
