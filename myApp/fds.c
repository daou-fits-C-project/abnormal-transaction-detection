#include <stdio.h>
#include <string.h>
#include <math.h>
#include "fds.h"
#include "db.h"
#include "util.h"

#define DEBUG

// 설정 변수
int day = 10;                     // 거래량 계산 기간
double stock_threshold = 2.58;    // 99% 신뢰도에 따른 Z Score
double ratio = 0.3;               // 특정 계좌 거래량 비율
double time_interval = 1800;      // 과거 주문 내역 간격
double lambda = 3;                // 단위 간격 동안의 주문 횟수
double lambda_threshold = 1.645;  // 95% 신뢰도에 따른 Z Score

static OCIStmt* stmthp;
static OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL, * def6 = NULL, * def7 = NULL, * def8 = NULL, * def9 = NULL;
static OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL, * bnd4 = NULL;

sword status;

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
    char* z_score_sql = "SELECT AVG(amount), STDDEV(amount) FROM transaction_log WHERE stock_id = :1 AND trade_date BETWEEN TRUNC(:2 - :3 - 1) AND TRUNC(:2 - 1) AND amount > 0";

    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)z_score_sql, strlen(z_score_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd3, errhp, 3, &day, sizeof(day), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &avg_amount, sizeof(avg_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def2, errhp, 2, &std_amount, sizeof(std_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    // 당일 총 거래량
    char* today_sql = "SELECT NVL(SUM(amount), 0) FROM normal_transaction WHERE stock_id = :1 AND TRUNC(created_at) = TRUNC(:2)";

    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)today_sql, strlen(today_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &today_amount, sizeof(today_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    double z_score = (today_amount + stock_order->amount - avg_amount) / std_amount;

#ifdef DEBUG
    printf("과거 %d일 동안의 거래량 평균: %.2lf, 표준편차: %.2lf\n", day, avg_amount, std_amount);
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
    char* customer_sql = "SELECT NVL(SUM(amount), 0) FROM normal_transaction WHERE account_id = :1 AND stock_id = :2 AND TRUNC(created_at) = TRUNC(:3)";
    
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)customer_sql, strlen(customer_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &stock_order->account_id, sizeof(stock_order->account_id), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd3, errhp, 3, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &customer_amount, sizeof(customer_amount), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    // 당일 총 거래량
    char* today_sql = "SELECT NVL(SUM(amount), 0) FROM normal_transaction WHERE stock_id = :1 AND TRUNC(created_at) = TRUNC(:2)";

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
    char* order_sql = "SELECT count(*) FROM stock_order WHERE account_id = :1 AND stock_id = :2 AND created_at BETWEEN TO_TIMESTAMP(:3, 'YYYY-MM-DD HH24:MI:SS') AND TO_TIMESTAMP(:4, 'YYYY-MM-DD HH24:MI:SS')";

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

    //double z_score = ((double)count - lambda) / sqrt(lambda);

#ifdef DEBUG
    printf("%s ~ %s\n", start_time, end_time);
    printf("%.lf초 동안의 주문 횟수: %d\n", time_interval, count);
    //printf("Z-Score: %.2lf\n", z_score);
#endif

    if (count >= lambda) return 0;
    return 1;
}

void report_FDS() {
    AbnormalTransaction records[100];
    char created_at[100];
    int index = 0;

    char* select_sql = "SELECT at.order_id, at.account_id, at.stock_id, at.price, at.amount, at.type, dt.type_name, dt.description, to_char(at.created_at, 'YYYY/MM/DD HH24:MI:SS') FROM abnormal_transaction at JOIN detection_type dt ON at.detection_id = dt.detection_id";

    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)select_sql, strlen(select_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);

    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    AbnormalTransaction abnormal_transaction;
    OCIDefineByPos(stmthp, &def1, errhp, 1, &abnormal_transaction.order_id, sizeof(abnormal_transaction.order_id), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def2, errhp, 2, &abnormal_transaction.account_id, sizeof(abnormal_transaction.account_id), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def3, errhp, 3, abnormal_transaction.stock_id, sizeof(abnormal_transaction.stock_id), SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def4, errhp, 4, &abnormal_transaction.price, sizeof(abnormal_transaction.price), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def5, errhp, 5, &abnormal_transaction.amount, sizeof(abnormal_transaction.amount), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def6, errhp, 6, &abnormal_transaction.type, sizeof(abnormal_transaction.type), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def7, errhp, 7, abnormal_transaction.type_name, sizeof(abnormal_transaction.type_name), SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def8, errhp, 8, abnormal_transaction.description, sizeof(abnormal_transaction.description), SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def9, errhp, 9, created_at, sizeof(created_at), SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);

    int flag = 0;
    while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT)) == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        datetime_to_tm(created_at, &abnormal_transaction.created_at);
        records[index++] = abnormal_transaction;
        flag = 1;
    }

    if (flag) {
        export_to_CSV(records, index);
        export_to_HTML(records, index);
    }
    else {
        printf("이상 거래 내역이 존재하지 않습니다.\n");
    }
}

void export_to_CSV(AbnormalTransaction* abnormal_records, int size) {
    FILE* file = fopen("FDS_report.csv", "w");
    if (!file) {
        printf("CSV 파일을 열 수 없습니다.\n");
        return 1;
    }

    fprintf(file, "order_id,account_id,stock_id,price,amount,type,type_name,description,created_at\n");
    for (int i = 0; i < size; i++) {
        fprintf(file, "%d,%d,%s,%d,%d,%s,%s,%s,%04d/%02d/%02d %02d:%02d:%02d\n",
            abnormal_records[i].order_id,
            abnormal_records[i].account_id,
            abnormal_records[i].stock_id,
            abnormal_records[i].price,
            abnormal_records[i].amount,
            abnormal_records[i].type ? "매수" : "매도",
            abnormal_records[i].type_name,
            abnormal_records[i].description,
            abnormal_records[i].created_at.tm_year + 1900, abnormal_records[i].created_at.tm_mon + 1, abnormal_records[i].created_at.tm_mday,
            abnormal_records[i].created_at.tm_hour, abnormal_records[i].created_at.tm_min, abnormal_records[i].created_at.tm_sec
        );
    }

    fclose(file);
}

void export_to_HTML(AbnormalTransaction* abnormal_records, int size) {
    FILE* file = fopen("FDS_report.html", "w");
    if (!file) {
        printf("HTML 파일을 열 수 없습니다.\n");
        return;
    }

    fprintf(file, "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>이상 거래 분석 보고서</title></head><body>");
    fprintf(file, "<h2>이상 거래 분석 보고서</h2>");
    fprintf(file, "<table><tr><th>order_id</th><th>account_id</th><th>stock_id</th><th>price</th><th>amount</th><th>type</th><th>type_name</th><th>description</th><th>created_at</th></tr>");

    for (int i = 0; i < size; i++) {
        fprintf(file, "<tr><td>%d</td><td>%d</td><td>%s</td><td>%d</td><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%04d/%02d/%02d %02d:%02d:%02d</td></tr>",
            abnormal_records[i].order_id,
            abnormal_records[i].account_id,
            abnormal_records[i].stock_id,
            abnormal_records[i].price,
            abnormal_records[i].amount,
            abnormal_records[i].type ? "매수" : "매도",
            abnormal_records[i].type_name,
            abnormal_records[i].description,
            abnormal_records[i].created_at.tm_year + 1900, abnormal_records[i].created_at.tm_mon + 1, abnormal_records[i].created_at.tm_mday,
            abnormal_records[i].created_at.tm_hour, abnormal_records[i].created_at.tm_min, abnormal_records[i].created_at.tm_sec
        );
    }

    fprintf(file, "</table></body></html>");
    fclose(file);

    system("start FDS_report.html");
}