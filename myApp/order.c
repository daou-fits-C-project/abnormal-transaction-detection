#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include "order.h"
#include "db.h"
#include "util.h"
#include "fds.h"
#include "account.h"
#include "transaction.h"
#include "stock.h"
#include "manage.h"


int add_order(StockOrder *order) {
    Account acc;
    Stock stock;
    get_account_by_id(&acc, order->account_id);
    get_stock_status(&stock, order->stock_id);
    if (acc.status != ACCOUNT_ACTIVE || stock.status != STOCK_ACTIVE) return 0;

    OCIStmt* stmthp;
    OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL, * bnd4 = NULL, * bnd5 = NULL, * bnd6 = NULL, * bnd7 = NULL;

    char* sql = get_sql("sql/order_queries.json", "insert");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    free(sql);

    // 처음은 PENDING 으로 설정
    order->status = PENDING;

    OCIBindByPos(stmthp, &bnd1, errhp, 1, order->stock_id, sizeof(order->stock_id), SQLT_STR,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &order->price, sizeof(order->price), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd3, errhp, 3, &order->amount, sizeof(order->amount), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd4, errhp, 4, &order->type, sizeof(order->type), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd5, errhp, 5, &order->status, sizeof(order->status), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd6, errhp, 6, &order->account_id, sizeof(order->account_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd7, errhp, 7, &order->order_id, sizeof(order->order_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
        OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
        check_error(errhp);
    }
    else {
        //printf("데이터 삽입 완료!\n");
        print_order(order);
    }

    return 1;

}

void print_order(const StockOrder* order) {
    char status[STATUS_BUF];
    char created_at[CREATED_AT_BUF];

    strcpy(status, order_status_to_string(order->status));
    tm_to_string(&order->created_at, created_at);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // 현재 콘솔 속성을 저장
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttr = csbi.wAttributes;

    WORD attributes = (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) | (order->type == BID ? BACKGROUND_GREEN : BACKGROUND_BLUE);
    // 속성 변경: attributes에는 원하는 전경/배경 색 조합을 지정
    SetConsoleTextAttribute(hConsole, attributes);

    printf(">> [%s] | 주문 ID: %5d | 주식 코드: %8s | 가격: %8d | 수량: %4d | 주문 상태: %10s | 계좌 ID : %4d | 주문 일자 : %s |\n",
        order_type_to_string(order->type),
        order->order_id, order->stock_id, order->price, order->amount,
        status, order->account_id, created_at);

    SetConsoleTextAttribute(hConsole, originalAttr);
}

void print_orders(StockOrder* order_arr, int count) {

    char status[STATUS_BUF];
    char created_at[CREATED_AT_BUF];

    printf("==================================================================================================================================================\n");
    printf("|                                                                  조회 결과                                                                     |\n");
    printf("==================================================================================================================================================\n");
    for (int i = 0; i < count; i++)
    {

        strcpy(status, order_status_to_string(order_arr[i].status));
        tm_to_string(&order_arr[i].created_at, created_at);
        printf("| 주문 ID: %5d | 주식 코드: %8s | 가격: %8d | 수량: %4d | 주문 상태: %10s | 계좌 ID : %4d | 주문 일자 : %s |\n",
            order_arr[i].order_id, order_arr[i].stock_id, order_arr[i].price, order_arr[i].amount,
            status, order_arr[i].account_id, created_at);
    }
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------\n");


    free(order_arr);
}

int get_order_count() {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL;
    int count = -1;
    char* sql = get_sql("sql/order_queries.json", "getCount");
    // SQL문 핸들을 생성
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. 데이터 가져오기 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        printf("\n데이터 개수 조회 결과: %d\n", count);
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
    // SQL문 핸들을 생성
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &account_id, sizeof(account_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. 데이터 가져오기 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        printf("\n데이터 개수 조회 결과: %d\n", count);
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
    // SQL문 핸들을 생성
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_id, sizeof(stock_id), SQLT_STR,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. 데이터 가져오기 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        printf("\n데이터 개수 조회 결과: %d\n", count);
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
    // SQL문 핸들을 생성
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &orderStatus, sizeof(orderStatus), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. 데이터 가져오기 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        printf("\n데이터 개수 조회 결과: %d\n", count);
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    return count;
}

int get_recent_order_count() {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL;
    int count = -1, max_result = 50;
    char* sql = get_sql("sql/order_queries.json", "getRecentCount");
    // SQL문 핸들을 생성
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &max_result, sizeof(max_result), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    free(sql);

    // 2. 쿼리 실행
    status = OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    if (status != OCI_SUCCESS) {
        check_error(errhp);
    }

    // 3. 데이터 바인딩 (결과를 받을 변수)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);

    // 4. 데이터 가져오기 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        //printf("\n데이터 개수 조회 결과: %d\n", count);
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

    // 1.  SQL문 핸들을 생성
    char* sql = get_sql("sql/order_queries.json", "selectAll");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
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

    // 4. 데이터 가져오기 
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
            printf("날짜 파싱 실패\n");
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

    // 1.  SQL문 핸들을 생성
    char* sql = get_sql("sql/order_queries.json", "selectByAccountId");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &account_id, sizeof(account_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
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

    // 4. 데이터 가져오기 
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
            printf("날짜 파싱 실패\n");
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

    // 1.  SQL문 핸들을 생성
    char* sql = get_sql("sql/order_queries.json", "selectByStockId");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_id, sizeof(stock_id), SQLT_STR,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
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

    // 4. 데이터 가져오기 
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
            printf("날짜 파싱 실패\n");
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

    // 1.  SQL문 핸들을 생성
    char* sql = get_sql("sql/order_queries.json", "selectByStatus");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &orderStatus, sizeof(orderStatus), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
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

    // 4. 데이터 가져오기 
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
            printf("날짜 파싱 실패\n");
            return;
        }
        idx++;
    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);
}

void get_recent_orders(StockOrder* order_arr) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL, * def6 = NULL, * def7 = NULL, * def8 = NULL;
    StockOrder order;
    char created_at[CREATED_AT_BUF];
    int max_result = 50;

    // 1.  SQL문 핸들을 생성
    char* sql = get_sql("sql/order_queries.json", "selectRecentOrder");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &max_result, sizeof(max_result), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
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

    // 4. 데이터 가져오기 
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
            printf("날짜 파싱 실패\n");
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


    // 바인딩 변수 설정 (UPDATE)
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &status, sizeof(status),
        SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &order_id, sizeof(order_id), SQLT_INT,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    // SQL 실행
    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
        OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
        check_error(errhp);
    }
    else {
        //printf("데이터 수정 완료!\n");
    }
}

int handle_client_order(const char* csv) {
    StockOrder order;
    int abnormal_order = 0;
    if (csv_string_to_order(csv, &order) != 0) {
        printf("주문 파싱 실패\n");
        return 0;
    };
    if (add_order(&order) == 0) { 
        //printf("주문 실패\n");
        return 0; 
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (strcmp(order.stock_id, "035720")) {
        if (!detect_stock_amount(&order)) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

            printf("\n[이상 탐지] 종목 거래량 이상(종목 정지: %s)\n", order.stock_id);
            printf("→ 특정 종목의 당일 거래량이 과거 평균 대비 비정상적으로 높습니다.\n\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            abnormal_order = 1;
        }
        else if (order.account_id == 1111 && !detect_account_amount(&order)) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

            printf("\n[이상 탐지] 계좌 거래량 이상(계좌 정지 ID: %d)\n", order.account_id);
            printf("→ 특정 계좌의 당일 거래량이 전체 대비 비정상적으로 많습니다.\n\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            abnormal_order = 2;
        }
        else if (!detect_wash_sale(&order)) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

            printf("\n[이상 탐지] 고빈도 주문 이상(계좌 정지 ID: %d)\n", order.account_id);
            printf("→ 특정 계좌에서 비정상적으로 반복적인 주문 활동이 감지되었습니다.\n\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            abnormal_order = 3;
        }
    }

    if (abnormal_order) {
        if (abnormal_order == 1) {
            update_stock_status(order.stock_id, STOCK_SUSPENDED);
        }
        else
        {
            update_account_status(order.account_id, ACCOUNT_SUSPENDED);
        }
        update_order_status(order.order_id, CANCELED);
        add_abnormal_transaction(&order, abnormal_order);  
        return 0;
    }
    else {
        update_order_status(order.order_id, MATCHED);
        add_normal_transaction(&order);
    }
    return 1;
}

void handle_order_monitoring() {
    int count = get_recent_order_count();
    StockOrder* order_arr = (StockOrder*)malloc(sizeof(StockOrder) * count);
    if (order_arr == NULL) {
        return;
    }
    get_recent_orders(order_arr);
    printf("                                      << 실시간 모니터링 중입니다. 모니터링을 종료하려면 'q' 또는 ESC 키를 누르세요 >>                                      \n");
    printf("============================================================================================================================================================\n");
    printf("|                                                                   주문 모니터링 시스템                                                                   |\n");
    printf("============================================================================================================================================================\n");
    for (int i = 0; i < count; i++) {
        print_order(&order_arr[i]);
    }
    fflush(stdout);  // 출력 버퍼 강제 플러시

    // 실시간 모니터링을 별도 스레드로 실행
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, real_time_monitoring, NULL, 0, NULL);
    if (hThread == 0) {
        printf("모니터링 스레드 생성 실패\n");
        free(order_arr);
        return;
    }

    while (!g_stopMonitoring) {
        Sleep(100);
    }

    // 모니터링 종료 후 스레드 종료 대기
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    free(order_arr);
    g_stopMonitoring = 0;
}

void handle_order() {
    StockOrder* order_arr = NULL;
    OrderStatus status;
    int choice, account_id, order_id, count = 0;
    char stock_id[STOCK_ID_BUF];
    while (1) {
        system("cls");
        printf("=====================================\n");
        printf("|                                   |\n");
        printf("|          주문 관리 시스템         |\n");
        printf("|                                   |\n");
        printf("=====================================\n");
        printf("|          메           뉴          |\n");
        printf("-------------------------------------\n");
        printf("|  1. 모든 주문 조회                |\n");
        printf("-------------------------------------\n");
        printf("|  2. 특정 계좌의 주문 조회         |\n");
        printf("-------------------------------------\n");
        printf("|  3. 특정 종목의 주문 조회         |\n");
        printf("-------------------------------------\n");
        printf("|  4. 주문 상태별 조회              |\n");
        printf("-------------------------------------\n");
        printf("|  5. 주문 모니터링                 |\n");
        printf("-------------------------------------\n");
        printf("|  6. 뒤로 가기                     |\n");
        printf("-------------------------------------\n");
        printf(">> 메뉴를 선택하세요 : ");
        scanf("%d", &choice);
        system("cls");
        switch (choice)
        {
        case 1:
            // 모든 주문 조회
            count = get_order_count();
            order_arr = (StockOrder*)malloc(sizeof(StockOrder) * count);
            if (order_arr == NULL)
            {
                printf("malloc 실패\n");
                return;
            }
            get_orders(order_arr);
            print_orders(order_arr, count);
            to_be_continue();
            break;
        case 2:
            // 특정 계좌 주문 조회
            printf("조회할 계좌 ID: ");
            scanf("%d", &account_id);
            count = get_order_count_by_account(account_id);
            order_arr = (StockOrder*)malloc(sizeof(StockOrder) * count);
            if (order_arr == NULL)
            {
                printf("malloc 실패\n");
                return;
            }
            get_orders_by_account(order_arr, account_id);
            print_orders(order_arr, count);
            to_be_continue();
            break;
        case 3:
            // 특정 종목 주문 조회
            printf("조회할 종목 코드: ");
            scanf("%s", stock_id);
            count = get_order_count_by_stock_id(stock_id);
            order_arr = (StockOrder*)malloc(sizeof(StockOrder) * count);
            if (order_arr == NULL)
            {
                printf("malloc 실패\n");
                return;
            }
            get_orders_by_stock_id(order_arr, stock_id);
            print_orders(order_arr, count);
            to_be_continue();
            break;
        case 4:
            printf("조회할 주문 상태(0: 미체결, 1: 체결, 2: 취소): ");
            scanf("%d", &status);
            count = get_order_count_by_status(status);
            order_arr = (StockOrder*)malloc(sizeof(StockOrder) * count);
            if (order_arr == NULL)
            {
                printf("malloc 실패\n");
                return;
            }
            get_orders_by_status(order_arr, status);
            print_orders(order_arr, count);
            to_be_continue();
            break;
        case 5:
            // printf("상태 변경할 주문 ID: ");
            // scanf("%d", &order_id);
            // printf("변경 주문 상태(0: 미체결, 1: 체결, 2: 취소): ");
            // scanf("%d", &status);
            // update_order_status(order_id, status);
            // break;

            // [변경] 주문 모니터링(최근 내역 조회, 최대 50개)
            handle_order_monitoring();
            break;
        case 6:
            return;
        default:
            printf("올바른 메뉴를 선택하세요.\n");
            to_be_continue();
            break;
        }
    }
}

const char* order_status_to_string(OrderStatus status) {
    switch (status) {
    case PENDING:   return "미체결";
    case MATCHED:   return "체결";
    case CANCELED:  return "취소";
    default:        return "알 수 없음";
    }
}

const char* order_type_to_string(OrderType type) {
    return type == BID ? "매수" : "매도";
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

    if (i < ORDER_TOKEN) return -1;  // 파싱 실패

    strncpy(order->stock_id, tokens[0], sizeof(order->stock_id));
    order->stock_id[sizeof(order->stock_id) - 1] = '\0';
    order->price = atoi(tokens[1]);
    order->amount = atoi(tokens[2]);
    order->type = parse_order_type(tokens[3]);
    order->account_id = atoi(tokens[4]);
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    if (t) {
        order->created_at = *t;
    }
    //printf("stock_id: %s\n", order->stock_id);
    return 0;
}