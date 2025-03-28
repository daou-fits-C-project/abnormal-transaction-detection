#include <stdio.h>
#include <string.h>
#include <math.h>
#include "fds.h"
#include "db.h"

#define DEBUG

// ���� ����
int month = 10;                   // �ŷ��� ��� �Ⱓ
double stock_threshold = 0.74;    // 77% �ŷڵ��� ���� Z Score
double ratio = 0.3;               // Ư�� ���� �ŷ��� ����
double time_interval = 1800;      // ���� �ֹ� ���� ����
double lambda = 2;                // ���� ���� ������ �ֹ� Ƚ��
double lambda_threshold = 1.645;  // 95% �ŷڵ��� ���� Z Score

static OCIStmt* stmthp;
static OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL, * def6 = NULL, * def7 = NULL, * def8 = NULL, * def9 = NULL;
static OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL, * bnd4 = NULL;

sword status;

int detect_stock_amount(StockOrder* stock_order) {
    // ���� 30�� ������ �ŷ���
    double avg_amount, std_amount;
    // ���� �� �ŷ���
    double today_amount;

    OCIDate today;
    struct tm* tm_info = &stock_order->created_at;
    today.OCIDateYYYY = stock_order->created_at.tm_year + 1900;
    today.OCIDateMM = stock_order->created_at.tm_mon + 1;
    today.OCIDateDD = stock_order->created_at.tm_mday;

    // ���� 30�� ������ �ŷ��� SQL
    char* z_score_sql = "SELECT AVG(amount), STDDEV(amount) FROM transaction_log WHERE stock_id = :1 AND trade_date BETWEEN TRUNC(:2 - :3) AND TRUNC(:2) AND amount > 0";

    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)z_score_sql, strlen(z_score_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd3, errhp, 3, &month, sizeof(month), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &avg_amount, sizeof(avg_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &def2, errhp, 2, &std_amount, sizeof(std_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    // ���� �� �ŷ���
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
    printf("���� %d�� ������ �ŷ��� ���: %.2lf, ǥ������: %.2lf\n", month, avg_amount, std_amount);
    printf("���� �� �ŷ��� : %.2lf\n", today_amount + stock_order->amount);
    printf("Z-Score: %.2lf\n", z_score);
#endif

    if (z_score > stock_threshold) return 0;
    return 1;
}

int detect_account_amount(StockOrder* stock_order) {
    // �ش� ������ �ŷ���
    int customer_amount;
    // ���� �� �ŷ���
    int total_amount;

    OCIDate today;
    struct tm* tm_info = &stock_order->created_at;
    today.OCIDateYYYY = stock_order->created_at.tm_year + 1900;
    today.OCIDateMM = stock_order->created_at.tm_mon + 1;
    today.OCIDateDD = stock_order->created_at.tm_mday;

    // �ش� ������ �ŷ��� SQL
    char* customer_sql = "SELECT NVL(SUM(amount), 0) FROM normal_transaction WHERE account_id = :1 AND stock_id = :2 AND TRUNC(created_at) = TRUNC(:3)";
    
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)customer_sql, strlen(customer_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &stock_order->account_id, sizeof(stock_order->account_id), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd3, errhp, 3, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &customer_amount, sizeof(customer_amount), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    // ���� �� �ŷ���
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
    printf("����: %d, ����: %s\n", stock_order->account_id, stock_order->stock_id);
    printf("���� �ŷ��� : %d, ���� �� �ŷ���: %d\n", customer_amount + stock_order->amount, total_amount + stock_order->amount);
    printf("Ratio: %.2lf\n", ratio_amount);
#endif

    if (ratio_amount >= ratio) return 0;
    return 1;
}

int detect_wash_sale(StockOrder* stock_order) {
    char start_time[20], end_time[20];  // "YYYY-MM-DD HH:MM:SS"
    int count = 0;

    // �ش� ������ 5�� �ֹ� �� ����
    char* order_sql = "SELECT count(*) FROM stock_order WHERE account_id = :1 AND stock_id = :2 AND created_at BETWEEN TO_TIMESTAMP(:3, 'YYYY-MM-DD HH24:MI:SS') AND TO_TIMESTAMP(:4, 'YYYY-MM-DD HH24:MI:SS')";

    // time_inverval �� �� ���
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
    printf("%s ~ %s\n", start_time, end_time);
    printf("%.lf�� ������ �ֹ� Ƚ��: %d\n", time_interval, count);
    printf("Z-Score: %.2lf\n", z_score);
#endif

    if (z_score >= lambda_threshold) return 0;
    return 1;
}

void report_FDS() {
    FILE* file = fopen("FDS_report.csv", "w");
    if (!file) {
        printf("������ �� �� �����ϴ�.\n");
        return 1;
    }
    fprintf(file, "order_id,account_id,stock_id,price,amount,type,type_name,description,created_at\n");

    char created_at[20];

    char* select_sql = "SELECT at.order_id, at.account_id, at.stock_id, at.price, at.amount, at.type, dt.type_name, dt.description FROM abnormal_transaction at JOIN detection_type dt ON at.detection_id = dt.detection_id";
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
    //OCIDefineByPos(stmthp, &def9, errhp, 9, created_at, sizeof(created_at), SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);

    int flag = 0;
    while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT)) == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        fprintf(file, "%d,%d,%s,%d,%d,%d,%s,%s\n",
            abnormal_transaction.order_id,
            abnormal_transaction.account_id,
            abnormal_transaction.stock_id,
            abnormal_transaction.price,
            abnormal_transaction.amount,
            abnormal_transaction.type,
            abnormal_transaction.type_name,
            abnormal_transaction.description
            //created_at
        );
        flag = 1;
    }

    if (!flag) printf("�̻� �ŷ� ������ �������� �ʽ��ϴ�.\n");

    fclose(file);
}