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
		printf("|          ������ ���α׷�          |\n");
		printf("|                                   |\n");
		printf("=====================================\n");
		printf("|          ��           ��          |\n");
		printf("-------------------------------------\n");
		printf("|  1. �� ��ȸ                     |\n");
		printf("-------------------------------------\n");
		printf("|  2. �ŷ� ���� ����                |\n");
		printf("-------------------------------------\n");
		printf("|  3. �ֹ� ���� ����                |\n");
		printf("-------------------------------------\n");
		printf("|  4. �̻� �ŷ� �м� ����         |\n");
		printf("-------------------------------------\n");
		printf("|  5. ����                          |\n");
		printf("-------------------------------------\n");
		printf(">> �޴��� �����ϼ��� : ");
		scanf("%d", &choice);
		system("cls");
		switch (choice) {
		case 1: select_all_with_accounts(); to_be_continue(); break;
		case 2: handle_transaction_log(); break;
		case 3: handle_order(); break;
		case 4: abnormal_analysis_report(); break;
		case 5: return;
		default: printf("�ùٸ� �޴��� �����ϼ���\n"); break;
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
		printf("|      �ŷ� ���� ���� ���α׷�      |\n");
		printf("|                                   |\n");
		printf("=====================================\n");
		printf("|          ��           ��          |\n");
		printf("-------------------------------------\n");
		printf("|  1. ���� �� �ŷ� ���� ��ȸ        |\n");
		printf("-------------------------------------\n");
		printf("|  2. ���� �� �ŷ� ���� ��ȸ        |\n");
		printf("-------------------------------------\n");
		printf("|  3. ���� ���� ����                |\n");
		printf("-------------------------------------\n");
		printf("|  4. ������ ���α׷����� ���ư���  |\n");
		printf("-------------------------------------\n");
		printf(">> �޴��� �����ϼ��� : ");
		scanf("%d", &choice);
		system("cls");
		switch (choice) {
		case 1: 
			printf(">> ã���� �ϴ� �ŷ� ���� ID : ");
			scanf("%d", &account_id);
			select_account_transaction(account_id); to_be_continue(); break;
		case 2: 
			printf(">> ã���� �ϴ� �ŷ� ���� �ڵ� : ");
			scanf("%s", stock_id);
			select_stock_transaction(stock_id); to_be_continue(); break;
		case 3: 
			printf(">> ������ ���� ID: ");
			scanf("%d", &account_id);
			printf(">> ���� �Է� (0: ����, 1: �Ͻ� ����, 2: ���, 3 :������, 4: ���� ��, 5: �����, 6: ���� �̿Ϸ�, 7: ���� ���� ");
			scanf("%d", &status);
			update_account_status(account_id, status);
			handle_order(); break;
		case 4: return;
		default: printf("�ùٸ� �޴��� �����ϼ���\n"); break;
		}
		system("cls");
	}
}

void abnormal_analysis_report() {

}

void to_be_continue() {
	char key;
	printf("��� �Ͻ÷��� ENTER�� ��������.. ");
	do {
		key = _getch();
	} while (key != 0x0d);
}

// �ǽð� ����͸� �Լ� (���� �����忡�� ����)
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
