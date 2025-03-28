#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "order.h"
#include "db.h"
#include "util.h"


void add_order(const char* csv) {
    StockOrder order;
    if (csv_string_to_order(csv, &order) != 0) {
        printf("�ֹ� �Ľ� ����\n");
        return;
    };
    OCIStmt* stmthp;
    OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL, * bnd4 = NULL, * bnd5 = NULL, * bnd6 = NULL;

    char* sql = get_sql("sql/order_queries.json", "insert");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    free(sql);

    // ó���� PENDING ���� ����
    int initialStatus = PENDING;

    OCIBindByPos(stmthp, &bnd1, errhp, 1, order.stock_id, sizeof(order.stock_id), SQLT_STR,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &order.price, sizeof(order.price), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd3, errhp, 3, &order.amount, sizeof(order.amount), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd4, errhp, 4, &order.type, sizeof(order.type), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd5, errhp, 5, &initialStatus, sizeof(initialStatus), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd6, errhp, 6, &order.account_id, sizeof(order.account_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
        OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
        check_error(errhp);
    }
    else {
        printf("������ ���� �Ϸ�!\n");
    }
}

void print_orders(StockOrder* order_arr, int count) {

    char status[STATUS_BUF];
    char created_at[CREATED_AT_BUF];

    printf("\n��ȸ ��� : \n");
    printf("--------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++)
    {

        strcpy(status, order_status_to_string(order_arr[i].status));
        tm_to_string(&order_arr[i].created_at, created_at);
        printf("�ֹ� ID: %5d, �ֽ� �ڵ�: %8s, ����: %8d, ����: %4d, �ֹ� ����: %10s, ���� ID : %4d, �ֹ� ���� : %s\n",
            order_arr[i].order_id, order_arr[i].stock_id, order_arr[i].price, order_arr[i].amount,
            status, order_arr[i].account_id, created_at);
    }
    printf("--------------------------------------------------------------------------------------------\n");


    free(order_arr);
}

int get_order_count() {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL;
    int count = -1;
    char* sql = get_sql("sql/order_queries.json", "getCount");
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
        printf("\n������ ���� ��ȸ ���: %d\n", count);
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    return count;
}

int get_order_count_by_account(int account_id) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL;
    int count = -1;
    char* sql = get_sql("sql/order_queries.json", "getCountByAccountId");
    // SQL�� �ڵ��� ����
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &account_id, sizeof(account_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. ������ �������� 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        printf("\n������ ���� ��ȸ ���: %d\n", count);
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    return count;
}

int get_order_count_by_stock_id(char* stock_id) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL;
    int count = -1;
    char* sql = get_sql("sql/order_queries.json", "getCountByStockId");
    // SQL�� �ڵ��� ����
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_id, sizeof(stock_id), SQLT_STR,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. ������ �������� 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        printf("\n������ ���� ��ȸ ���: %d\n", count);
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    return count;
}

int get_order_count_by_status(OrderStatus orderStatus) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL;
    int count = -1;
    char* sql = get_sql("sql/order_queries.json", "getCountByStatus");
    // SQL�� �ڵ��� ����
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &orderStatus, sizeof(orderStatus), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. ������ �������� 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        printf("\n������ ���� ��ȸ ���: %d\n", count);
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    return count;
}

void get_orders(StockOrder *order_arr) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL, * def6 = NULL, * def7 = NULL, * def8 = NULL;
    StockOrder order;
    char created_at[CREATED_AT_BUF];

    // 1.  SQL�� �ڵ��� ����
    char* sql = get_sql("sql/order_queries.json", "selectAll");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def2, errhp, 1, &order.price, sizeof(order.price), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def3, errhp, 2, &order.amount, sizeof(order.amount), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def4, errhp, 3, &order.type, sizeof(order.type),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def5, errhp, 4, &order.status, sizeof(order.status),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def6, errhp, 5, &order.account_id, sizeof(order.account_id),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def7, errhp, 6, &order.stock_id, sizeof(order.stock_id), SQLT_STR, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def8, errhp, 7, created_at, sizeof(created_at),
        SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def1, errhp, 8, &order.order_id, sizeof(order.order_id), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. ������ �������� 
    int idx = 0;
    while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        order_arr[idx].order_id = order.order_id;
        strcpy(order_arr[idx].stock_id, order.stock_id);
        order_arr[idx].price = order.price;
        order_arr[idx].amount = order.amount;
        order_arr[idx].account_id = order.account_id;
        order_arr[idx].type = order.type;
        order_arr[idx].status = order.status;
        if (datetime_to_tm(created_at, &order_arr[idx].created_at) != 0) {
            printf("��¥ �Ľ� ����\n");
            return;
        }
        idx++;
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);
}

void get_orders_by_account(StockOrder *order_arr, int account_id) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL, * def6 = NULL, * def7 = NULL, * def8 = NULL;
    StockOrder order;
    char created_at[CREATED_AT_BUF];

    // 1.  SQL�� �ڵ��� ����
    char* sql = get_sql("sql/order_queries.json", "selectByAccountId");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &account_id, sizeof(account_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def2, errhp, 1, &order.price, sizeof(order.price), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def3, errhp, 2, &order.amount, sizeof(order.amount), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def4, errhp, 3, &order.type, sizeof(order.type),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def5, errhp, 4, &order.status, sizeof(order.status),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def6, errhp, 5, &order.account_id, sizeof(order.account_id),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def7, errhp, 6, &order.stock_id, sizeof(order.stock_id), SQLT_STR, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def8, errhp, 7, created_at, sizeof(created_at),
        SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def1, errhp, 8, &order.order_id, sizeof(order.order_id), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. ������ �������� 
    int idx = 0;
    while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        order_arr[idx].order_id = order.order_id;
        strcpy(order_arr[idx].stock_id, order.stock_id);
        order_arr[idx].price = order.price;
        order_arr[idx].amount = order.amount;
        order_arr[idx].account_id = order.account_id;
        order_arr[idx].type = order.type;
        order_arr[idx].status = order.status;

        if (datetime_to_tm(created_at, &order_arr[idx].created_at) != 0) {
            printf("��¥ �Ľ� ����\n");
            return;
        }
        idx++;
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);
}

void get_orders_by_stock_id(StockOrder* order_arr, char* stock_id) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL, * def6 = NULL, * def7 = NULL, * def8 = NULL;
    StockOrder order;
    char created_at[CREATED_AT_BUF];

    // 1.  SQL�� �ڵ��� ����
    char* sql = get_sql("sql/order_queries.json", "selectByStockId");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_id, sizeof(stock_id), SQLT_STR,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def2, errhp, 1, &order.price, sizeof(order.price), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def3, errhp, 2, &order.amount, sizeof(order.amount), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def4, errhp, 3, &order.type, sizeof(order.type),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def5, errhp, 4, &order.status, sizeof(order.status),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def6, errhp, 5, &order.account_id, sizeof(order.account_id),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def7, errhp, 6, &order.stock_id, sizeof(order.stock_id), SQLT_STR, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def8, errhp, 7, created_at, sizeof(created_at),
        SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def1, errhp, 8, &order.order_id, sizeof(order.order_id), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. ������ �������� 
    int idx = 0;
    while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        order_arr[idx].order_id = order.order_id;
        strcpy(order_arr[idx].stock_id, order.stock_id);
        order_arr[idx].price = order.price;
        order_arr[idx].amount = order.amount;
        order_arr[idx].account_id = order.account_id;
        order_arr[idx].type = order.type;
        order_arr[idx].status = order.status;

        if (datetime_to_tm(created_at, &order_arr[idx].created_at) != 0) {
            printf("��¥ �Ľ� ����\n");
            return;
        }
        idx++;
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);
}

void get_orders_by_status(StockOrder *order_arr, OrderStatus orderStatus) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL, * def6 = NULL, * def7 = NULL, * def8 = NULL;
    StockOrder order;
    char created_at[CREATED_AT_BUF];

    // 1.  SQL�� �ڵ��� ����
    char* sql = get_sql("sql/order_queries.json", "selectByStatus");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &orderStatus, sizeof(orderStatus), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. ���� ����
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. ������ ���ε� (����� ���� ����)
    OCIDefineByPos(stmthp, &def2, errhp, 1, &order.price, sizeof(order.price), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def3, errhp, 2, &order.amount, sizeof(order.amount), SQLT_INT, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def4, errhp, 3, &order.type, sizeof(order.type),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def5, errhp, 4, &order.status, sizeof(order.status),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def6, errhp, 5, &order.account_id, sizeof(order.account_id),
        SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def7, errhp, 6, &order.stock_id, sizeof(order.stock_id), SQLT_STR, NULL,
        NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def8, errhp, 7, created_at, sizeof(created_at),
        SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def1, errhp, 8, &order.order_id, sizeof(order.order_id), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. ������ �������� 
    int idx = 0;
    while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        order_arr[idx].order_id = order.order_id;
        strcpy(order_arr[idx].stock_id, order.stock_id);
        order_arr[idx].price = order.price;
        order_arr[idx].amount = order.amount;
        order_arr[idx].account_id = order.account_id;
        order_arr[idx].type = order.type;
        order_arr[idx].status = order.status;
        if (datetime_to_tm(created_at, &order_arr[idx].created_at) != 0) {
            printf("��¥ �Ľ� ����\n");
            return;
        }
        idx++;
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);
}

void update_order_status(int order_id, OrderStatus status) {
    OCIStmt* stmthp;
    OCIBind* bnd1 = NULL, * bnd2 = NULL;

    char* sql = get_sql("sql/order_queries.json", "updateStatusById");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql),
        OCI_NTV_SYNTAX, OCI_DEFAULT);
    free(sql);


    // ���ε� ���� ���� (UPDATE)
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &status, sizeof(status),
        SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &order_id, sizeof(order_id), SQLT_INT,
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


void handle_order() {
    StockOrder* order_arr = NULL;
    OrderStatus status;
    int choice, account_id, order_id, count = 0;
    char stock_id[STOCK_ID_BUF];
    int exit_flag = 0;
    while (1)
    {
        if (exit_flag) break;
        printf("=====�ֹ� ���� �ý���=====\n");
        printf("1. ��� �ֹ� ��ȸ\n");
        printf("2. Ư�� ������ �ֹ� ��ȸ\n");
        printf("3. Ư�� ������ �ֹ� ��ȸ\n");
        printf("4. �ֹ� ���º� ��ȸ\n");
        printf("5. �ֹ� ���� ����\n");
        printf("6. ����\n");
        printf("\n����: ");
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:
                // ��� �ֹ� ��ȸ
                count = get_order_count();
                order_arr = (StockOrder*)malloc(sizeof(StockOrder) * count);
                if (order_arr == NULL)
                {
                    printf("malloc ����\n");
                    return;
                }
                get_orders(order_arr);
                print_orders(order_arr, count);
                break;
            case 2:
                // Ư�� ���� �ֹ� ��ȸ
                printf("��ȸ�� ���� ID: ");
                scanf("%d", &account_id);
                count = get_order_count_by_account(account_id);
                order_arr = (StockOrder*)malloc(sizeof(StockOrder) * count);
                if (order_arr == NULL)
                {
                    printf("malloc ����\n");
                    return;
                }
                get_orders_by_account(order_arr, account_id);
                print_orders(order_arr, count);
                break;
            case 3:
                // Ư�� ���� �ֹ� ��ȸ
                printf("��ȸ�� ���� ID: ");
                scanf("%s", stock_id);
                count = get_order_count_by_stock_id(stock_id);
                order_arr = (StockOrder*)malloc(sizeof(StockOrder) * count);
                if (order_arr == NULL)
                {
                    printf("malloc ����\n");
                    return;
                }
                get_orders_by_stock_id(order_arr, stock_id);
                print_orders(order_arr, count);
                break;
            case 4:
                printf("��ȸ�� �ֹ� ����(0: ��ü��, 1: ü��, 2: ���): ");
                scanf("%d", &status);
                count = get_order_count_by_status(status);
                order_arr = (StockOrder*)malloc(sizeof(StockOrder) * count);
                if (order_arr == NULL)
                {
                    printf("malloc ����\n");
                    return;
                }
                get_orders_by_status(order_arr, status);
                print_orders(order_arr, count);
                break;
            case 5:
                printf("���� ������ �ֹ� ID: ");
                scanf("%d", &order_id);
                printf("���� �ֹ� ����(0: ��ü��, 1: ü��, 2: ���): ");
                scanf("%d", &status);
                update_order_status(order_id, status);
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

const char* order_status_to_string(OrderStatus status) {
    switch (status) {
    case PENDING:   return "��ü��";
    case MATCHED:   return "ü��";
    case CANCELED:  return "���";
    default:        return "�� �� ����";
    }
}

const char* order_type_to_string(OrderType type) {
    return type == BID ? "�ż�" : "�ŵ�";
}

void order_to_csv_string(const StockOrder* order, char* buffer, size_t bufsize) {
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &order->created_at);

    snprintf(buffer, bufsize, "%d,%.2f,%d,%s,%s,%d,%s,%s",
        order->order_id,
        order->price,
        order->amount,
        order_type_to_string(order->type),
        order_status_to_string(order->status),
        order->account_id,
        order->stock_id,
        timebuf);
}


OrderType parse_order_type(const char* str) {
    return strcmp(str, "BUY") == 0 ? BID : ASK;
}

OrderStatus parse_order_status(const char* str) {
    if (strcmp(str, "PENDING") == 0) return PENDING;
    if (strcmp(str, "MATCHED") == 0) return MATCHED;
    if (strcmp(str, "CANCELED") == 0) return CANCELED;
    return PENDING;
}

int csv_string_to_order(const char* csv, StockOrder* order) {
    char temp[256];
    strncpy(temp, csv, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    char* tokens[ORDER_TOKEN];
    int i = 0;
    char* tok = strtok(temp, ",");
    while (tok && i < ORDER_TOKEN) {
        tokens[i++] = tok;
        tok = strtok(NULL, ",");
    }

    if (i < ORDER_TOKEN) return -1;  // �Ľ� ����

    strncpy(order->stock_id, tokens[0], sizeof(order->stock_id));
    order->price = atoi(tokens[1]);
    order->amount = atoi(tokens[2]);
    order->type = parse_order_type(tokens[3]);
    order->account_id = atoi(tokens[4]);
    order->stock_id[sizeof(order->stock_id) - 1] = '\0';

    printf("stock_id: %s\n", order->stock_id);;


    return 0;
}