#pragma once
#include <time.h>
#define STATUS_BUF 32
typedef enum {
    ACCOUNT_ACTIVE,            // 정상 사용 중
    ACCOUNT_SUSPENDED,         // 일시 정지 (고객 요청 or 이상거래 감지)
    ACCOUNT_LOCKED,            // 비밀번호 오류/접근 제한 등으로 잠김
    ACCOUNT_CLOSED,            // 고객 요청으로 완전 해지
    ACCOUNT_PENDING,           // 개설 진행 중 (KYC 심사, 인증 등)
    ACCOUNT_FROZEN,            // 법적 문제, 압류 등으로 동결
    ACCOUNT_UNVERIFIED,        // 인증 미완료 (비대면 가입 중 등)
    ACCOUNT_RESTRICTED         // 특정 거래 제한 (예: 해외주식 금지 등)
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