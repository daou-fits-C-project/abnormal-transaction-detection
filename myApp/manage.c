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
		case 1: select_all_with_accounts(); break;
		case 2: handle_transaction_log(); break;
		case 3: handle_order(); break;
		case 4: abnormal_analysis_report(); break;
		case 5: return;
		default: printf("�ùٸ� �޴��� �����ϼ���\n"); break;
		}
		char key;
		do {
			printf("��� �Ͻ÷��� ENTER�� ��������.. ");
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