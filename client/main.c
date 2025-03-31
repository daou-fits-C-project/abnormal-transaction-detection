#include "order.h"

void run_user() {
    int choice;
    while (1) {
        printf("=====================================\n");
        printf("|                                   |\n");
        printf("|           유저 프로그램           |\n");
        printf("|                                   |\n");
        printf("=====================================\n");
        printf("|          메           뉴          |\n");
        printf("-------------------------------------\n");
        printf("|  1. 정상 주문                     |\n");
        printf("-------------------------------------\n");
        printf("|  2. 비정상 주문[1]                |\n");
        printf("-------------------------------------\n");
        printf("|  3. 비정상 주문[2]                |\n");
        printf("-------------------------------------\n");
        printf("|  4. 비정상 주문[3]                |\n");
        printf("-------------------------------------\n");
        printf("|  5. 종료                          |\n");
        printf("-------------------------------------\n");
        printf(">> 메뉴를 선택하세요 : ");
        scanf("%d", &choice);
        system("cls");
        switch (choice) {
        case 1: run_normal_order(); break;
        case 2: run_abnormal_order_1(); break;
        case 3: run_abnormal_order_2(); break;
        case 4: run_abnormal_order_3(); break;
        case 5: return;
        default: printf("올바른 메뉴를 선택하세요\n"); break;
        }
        system("cls");
    }
}

int main() {

    run_user();
   
}
