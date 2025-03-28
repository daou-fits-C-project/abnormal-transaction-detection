#include <stdio.h>
#include <string.h>
#include <math.h>
#include "fds.h"
#include "db.h"

#define DEBUG

// 설정 변수
int month = 30;                   // 거래량 계산 기간
double stock_threshold = 0.674;   // 75% 신뢰도에 따른 Z Score
double ratio = 0.7;               // 특정 계좌 거래량 비율
double time_interval = 300;       // 과거 주문 내역 간격
double lambda = 1;                // 단위 간격 동안의 주문 횟수
double lambda_threshold = 1.645;  // 95% 신뢰도에 따른 Z Score

static OCIStmt* stmthp;
static OCIDefine* def1 = NULL, * def2 = NULL;
static OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL, * bnd4 = NULL;

int detect_stock_amount(StockOrder* stock_order) {
    // 과거 30일 동안의 거래량
    double avg_amount, std_amount;
    // 당일 총 거래량
    double today_amount;

    OCIDate today;
    struct tm* tm_info = &stock_order->created_at;
    today.OCIDateYYYY = stock_order->created_at.tm_year + 1900;
    today.OCIDateMM = stock_order->created_at.tm_mon + 1;
    today.OCIDateDD = stock_order->created_at.tm_mday;

    // 과거 30일 동안의 거래량 SQL
    char* z_score_sql = get_sql("sql/fbs_queries.json", "getAmountStatsInPeriod");

    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)z_score_sql, strlen(z_score_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd3, errhp, 3, &month, sizeof(month), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &avg_amount, sizeof(avg_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def2, errhp, 2, &std_amount, sizeof(std_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    // 당일 총 거래량
    char* today_sql = get_sql("sql/fbs_queries.json", "getAmountToday");

    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)today_sql, strlen(today_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &today_amount, sizeof(today_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    double z_score = (today_amount + stock_order->amount - avg_amount) / std_amount;

#ifdef DEBUG
    printf("과거 30일 동안의 거래량 평균: %.2lf, 표준편차: %.2lf\n", avg_amount, std_amount);
    printf("당일 총 거래량 : %.2lf\n", today_amount + stock_order->amount);
    printf("Z-Score: %.2lf\n", z_score);
#endif

    if (z_score > stock_threshold) return 0;
    return 1;
}

int detect_account_amount(StockOrder* stock_order) {
    // 해당 계좌의 거래량
    int customer_amount;
    // 당일 총 거래량
    int total_amount;

    OCIDate today;
    struct tm* tm_info = &stock_order->created_at;
    today.OCIDateYYYY = stock_order->created_at.tm_year + 1900;
    today.OCIDateMM = stock_order->created_at.tm_mon + 1;
    today.OCIDateDD = stock_order->created_at.tm_mday;

    // 해당 계좌의 거래량 SQL
    char* customer_sql = get_sql("sql/fbs_queries.json", "getAmountByAccountId");
    
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)customer_sql, strlen(customer_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &stock_order->account_id, sizeof(stock_order->account_id), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd3, errhp, 3, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &customer_amount, sizeof(customer_amount), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    // 당일 총 거래량
    char* today_sql = get_sql("sql/fbs_queries.json", "getAmountToday");

    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)today_sql, strlen(today_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &total_amount, sizeof(total_amount), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    double ratio_amount = (double)(customer_amount + stock_order->amount) / (double)(total_amount + stock_order->amount);

#ifdef DEBUG
    printf("계좌: %d, 종목: %s\n", stock_order->account_id, stock_order->stock_id);
    printf("계좌 거래량 : %d, 당일 총 거래량: %d\n", customer_amount + stock_order->amount, total_amount + stock_order->amount);
    printf("Ratio: %.2lf\n", ratio_amount);
#endif

    if (ratio_amount >= ratio) return 0;
    return 1;
}

int detect_wash_sale(StockOrder* stock_order) {
    char start_time[20], end_time[20];  // "YYYY-MM-DD HH:MM:SS"
    int count = 0;

    // 해당 계좌의 5분 주문 수 쿼리
    char* order_sql = get_sql("sql/fbs_queries.json", "getCountByFiveMin");

    // time_inverval 초 전 계산
    strftime(end_time, sizeof(end_time), "%Y-%m-%d %H:%M:%S", &stock_order->created_at);
    time_t past_time = mktime(&stock_order->created_at) - time_interval;
    struct tm* past_tm = localtime(&past_time);
    strftime(start_time, sizeof(start_time), "%Y-%m-%d %H:%M:%S", past_tm);

    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)order_sql, strlen(order_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, &stock_order->account_id, sizeof(stock_order->account_id), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd3, errhp, 3, &start_time, sizeof(start_time), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd4, errhp, 4, &end_time, sizeof(end_time), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &count, sizeof(count), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    double z_score = ((double)count - lambda) / sqrt(lambda);

#ifdef DEBUG
    printf("%.lf초 동안의 주문 횟수: %d\n", time_interval, count);
    printf("Z-Score: %.2lf\n", z_score);
#endif

    if (z_score >= lambda_threshold) return 0;
    return 1;
}