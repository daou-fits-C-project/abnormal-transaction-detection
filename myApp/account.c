#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "account.h"
#include "util.h"
int get_accounts_count() {
    OCIStmt* stmthp;
    sword status;
    OCIDefine* def1 = NULL;
    int count = -1;
    char* sql = get_sql("sql/account_queries.json", "getCount");

    // SQL�� �ڵ��� ����
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);
    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. ������ �������� 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        printf("������ ���� ��ȸ ���: %d\n", count);
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    return count;
}

void get_accounts(Account* acc_arr) {
    OCIStmt* stmthp;
    sword status;
    OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL;
    Account acc;
    char created_at[CREATED_AT_BUF];

    // 1.  SQL�� �ڵ��� ����
    char* sql = get_sql("sql/account_queries.json", "selectAll");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &acc.account_id, sizeof(acc.account_id), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def2, errhp, 2, &acc.balance, sizeof(acc.balance), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def3, errhp, 3, &acc.status, sizeof(acc.status), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def4, errhp, 4, &acc.customer_id, sizeof(acc.customer_id), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def5, errhp, 5, created_at, sizeof(created_at), SQLT_STR, NULL,
        NULL, NULL, OCI_DEFAULT);
    



    // 4. ������ �������� 
    int idx = 0;
    while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        acc_arr[idx].account_id = acc.account_id;
        acc_arr[idx].balance = acc.balance;
        acc_arr[idx].status = acc.status;
        acc_arr[idx].customer_id = acc.customer_id;

        if (datetime_to_tm(created_at, &acc_arr[idx].created_at) != 0) {
            printf("��¥ �Ľ� ����\n");
            return;
        }

        idx++;
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);
}


void get_account_by_id(Account* acc_ptr, int account_id) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL;
    Account acc;
    char created_at[CREATED_AT_BUF];

    // 1.  SQL�� �ڵ��� ����
    char* sql = get_sql("sql/account_queries.json", "selectByAccountId");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &account_id, sizeof(account_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &acc.account_id, sizeof(acc.account_id), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def2, errhp, 2, &acc.balance, sizeof(acc.balance), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def3, errhp, 3, &acc.status, sizeof(acc.status), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def4, errhp, 4, &acc.customer_id, sizeof(acc.account_id), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def5, errhp, 5, created_at, sizeof(created_at), SQLT_STR, NULL,
        NULL, NULL, OCI_DEFAULT);



    // 4. ������ �������� 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        acc_ptr->account_id = acc.account_id;
        acc_ptr->balance = acc.balance;
        acc_ptr->status = acc.status;
        acc_ptr->customer_id = acc.customer_id;
   
        if (datetime_to_tm(created_at, &acc_ptr->created_at) != 0) {
            printf("��¥ �Ľ� ����\n");
            return;
        }
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);
}

void print_accounts(Account* acc_arr, int count) {

    char status[STATUS_BUF];
    char created_at[CREATED_AT_BUF];
    
    printf("��ȸ ��� : \n");
    printf("--------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++)
    {
        strcpy(status, account_status_to_string(acc_arr[i].status));
        tm_to_string(&acc_arr[i].created_at, created_at);
        printf("���� ID: %4d, �ܾ�: %8d, ���� ����: %10s, �� ID: %4d, ������: %s\n",
            acc_arr[i].account_id, acc_arr[i].balance, status, acc_arr[i].customer_id, created_at);
    }
    printf("--------------------------------------------------------------------------------------------\n");


    free(acc_arr);
}


void add_account(int customer_id) {

    OCIStmt* stmthp;
    OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL, * bnd4 = NULL, * bnd5 = NULL;

    char* sql = get_sql("sql/account_queries.json", "insert");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    free(sql);

    int initialStatus = 1;

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &initialStatus, sizeof(initialStatus), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &customer_id, sizeof(customer_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
        OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
        check_error(errhp);
    }
    else {
        printf("������ ���� �Ϸ�!\n");
    }

}


void update_account_status(int account_id, AccountStatus status) {
    OCIStmt* stmthp;
    OCIBind* bnd1 = NULL, * bnd2 = NULL;

    char* sql = get_sql("sql/account_queries.json", "updateStatusById");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql),
        OCI_NTV_SYNTAX, OCI_DEFAULT);
    free(sql);


    // ���ε� ���� ���� (UPDATE)
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &status, sizeof(status),
        SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &account_id, sizeof(account_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    // SQL ����
    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
        OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
        check_error(errhp);
    }
    else {
        printf("������ ���� �Ϸ�!\n");
    }
}


void update_account_balance(int account_id, int amount) {
    OCIStmt* stmthp;
    OCIBind* bnd1 = NULL, * bnd2 = NULL;

    Account acc;

    get_account_by_id(&acc, account_id);
    printf("balance ��: %d\n", acc.balance);
    acc.balance += amount;
    if (acc.balance < 0)
    {
            printf("����� �Ұ��մϴ�\n");
            return;
    }
    printf("balance ��: %d\n", acc.balance);

    char* sql = get_sql("sql/account_queries.json", "updateBalanceById");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql),
        OCI_NTV_SYNTAX, OCI_DEFAULT);
    free(sql);


    // ���ε� ���� ���� (UPDATE)
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &acc.balance, sizeof(acc.balance),
        SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &account_id, sizeof(account_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    // SQL ����
    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
        OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
        check_error(errhp);
    }
    else {
        printf("������ ���� �Ϸ�!\n");
    }
}


const char* account_status_to_string(AccountStatus status) {
    switch (status) {
    case ACCOUNT_ACTIVE: return "����";
    case ACCOUNT_SUSPENDED: return "�Ͻ� ����";
    case ACCOUNT_LOCKED: return "���";
    case ACCOUNT_CLOSED: return "������";
    case ACCOUNT_PENDING: return "���� ��";
    case ACCOUNT_FROZEN: return "�����";
    case ACCOUNT_UNVERIFIED: return "���� �̿Ϸ�";
    case ACCOUNT_RESTRICTED: return "���� ����";
    default: return "�� �� ����";
    }
}



void handle_account() {
    Account* acc_arr = NULL;
    int choice, account_id, status, count = 0, transaction_type, amount;
    int exit_flag = 0;
    while (1)
    {
        if (exit_flag) break;
        printf("=====���� ���� �ý���=====\n");
        printf("1. ���� �߰�\n");
        printf("2. ��� ���� ��ȸ\n");
        printf("3. ���� �˻�\n");
        printf("4. ���� ���� ����\n");
        printf("5. ���� �ܾ� ����(�� ���)\n");
        printf("6. ����\n");
        printf("����: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            // ���� �α��ε� ���� ���� ����(���� �ʿ�) 
            add_account(1);
            break;
        case 2:
            // ��� ���� ��ȸ
            count = get_accounts_count();
            acc_arr = (Account*)malloc(sizeof(Account) * count);
            if (acc_arr == NULL)
            {
                printf("malloc ����\n");
                return;
            }
            get_accounts(acc_arr);
            print_accounts(acc_arr, count);
            break;
        case 3:
            // Ư�� ���� �˻�
            count = 1;
            acc_arr = (Account*)malloc(sizeof(Account) * count);
            printf("�˻��� ���� ID: ");
            scanf("%d", &account_id);
            get_account_by_id(acc_arr, account_id);
            print_accounts(acc_arr, count);
            break;
        case 4:
            // ���� ���� ����
            printf("������ ���� ID: ");
            scanf("%d", &account_id);
            printf("���� �Է� (0: ����, 1: �Ͻ� ����, 2: ���, 3 :������, 4: ���� ��, 5: �����, 6: ���� �̿Ϸ�, 7: ���� ���� ");
            scanf("%d", &status);
            update_account_status(account_id, status);
            break;
        case 5:
            // �����
            printf("������ ���� ID: ");
            scanf("%d", &account_id);
            printf("����� ���� (1: �Ա�, -1: ���)");
            scanf("%d", &transaction_type);
            printf("�ݾ�");
            scanf("%d", &amount);
            update_account_balance(account_id, amount * transaction_type);
            break;
        case 6:
            exit_flag = 1;
            break;
        default:
            printf("�߸��� �����Դϴ�\n");
            break;
        }
    }
}