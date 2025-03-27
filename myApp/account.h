#pragma once
#include <time.h>
#define STATUS_BUF 32
typedef enum {
    ACCOUNT_ACTIVE,            // ���� ��� ��
    ACCOUNT_SUSPENDED,         // �Ͻ� ���� (�� ��û or �̻�ŷ� ����)
    ACCOUNT_LOCKED,            // ��й�ȣ ����/���� ���� ������ ���
    ACCOUNT_CLOSED,            // �� ��û���� ���� ����
    ACCOUNT_PENDING,           // ���� ���� �� (KYC �ɻ�, ���� ��)
    ACCOUNT_FROZEN,            // ���� ����, �з� ������ ����
    ACCOUNT_UNVERIFIED,        // ���� �̿Ϸ� (���� ���� �� ��)
    ACCOUNT_RESTRICTED         // Ư�� �ŷ� ���� (��: �ؿ��ֽ� ���� ��)
} AccountStatus;


typedef struct {
    int account_id;
    int balance;
    AccountStatus status;
    int customer_id;
    struct tm created_at;
} Account;


int get_accounts_count();
void get_accounts(Account* acc_arr);
void get_account_by_id(Account* acc_ptr, int account_id);
void print_accounts();
void add_account(int customer_id);
void update_account_status(int account_id, AccountStatus status);
void update_account_balance(int account_id, int amount);
const char* account_status_to_string(AccountStatus status);
void handle_account();