#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "manage.h"

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
		case 1: select_all_with_accounts(); break;
		case 2: handle_transaction_log(); break;
		case 3: handle_order(); break;
		case 4: abnormal_analysis_report(); break;
		case 5: return;
		default: printf("올바른 메뉴를 선택하세요\n"); break;
		}
		char key;
		do {
			printf("계속 하시려면 ENTER를 누르세요.. ");
			key = _getch();
		} while (key != 0x0d);

		system("cls");
	}
}

void handle_transaction_log() {

}

void handle_order() {

}

void abnormal_analysis_report() {

}