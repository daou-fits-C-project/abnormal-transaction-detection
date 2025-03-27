#include <stdio.h>
#include <string.h>
#include "fds.h"
#include "db.h"

#define DEBUG

// ���� ����
int month = 30;            // �ŷ��� ��� �Ⱓ
double threshold = 0.674;  // 75% �ŷڵ��� ���� Z Score
double ratio = 0.7;        // Ư�� ���� �ŷ��� ����

static OCIStmt* stmthp;
static OCIDefine* def1 = NULL, * def2 = NULL;
static OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL;

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
    char* z_score_sql = "SELECT AVG(amount), STDDEV(amount) FROM transaction_log WHERE stock_id = :1 AND trade_date BETWEEN TRUNC(:2 - :3) AND TRUNC(:2)";

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

    double z_score = (today_amount + stock_order->amount - avg_amount) / std_amount;

#ifdef DEBUG
    printf("���� 30�� ������ �ŷ��� ���: %.2lf, ǥ������: %.2lf\n", avg_amount, std_amount);
    printf("���� �� �ŷ��� : %.2lf\n", today_amount + stock_order->amount);
    printf("Z-Score: %.2lf\n", z_score);
#endif

    if (z_score > threshold) return 0;
    return 1;
}

int detect_account_amount(StockOrder* stock_order) {
    // �ش� ������ �ŷ���
    double customer_amount;
    // ���� �� �ŷ���
    double total_amount;
    // �ŷ��� ����
    double ratio_amount = 0;

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

    OCIDefineByPos(stmthp, &def1, errhp, 1, &customer_amount, sizeof(customer_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);
    OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    // ���� �� �ŷ���
    char* today_sql = "SELECT NVL(SUM(amount), 0) FROM normal_transaction WHERE stock_id = :1 AND TRUNC(created_at) = TRUNC(:2)";

    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

    OCIStmtPrepare(stmthp, errhp, (text*)today_sql, strlen(today_sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_order->stock_id, sizeof(stock_order->stock_id), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, &today, sizeof(today), SQLT_ODT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &def1, errhp, 1, &total_amount, sizeof(total_amount), SQLT_BDOUBLE, NULL, NULL, NULL, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) check_error(errhp);

    if (total_amount != 0) {
        ratio_amount = customer_amount / total_amount;
    }

#ifdef DEBUG
    printf("����: %d, ����: %s\n", stock_order->account_id, stock_order->stock_id);
    printf("���� �ŷ��� : %.2lf, ���� �� �ŷ���: %.2lf\n", customer_amount + stock_order->amount, total_amount + stock_order->amount);
    printf("Ratio: %.2lf\n", ratio_amount);
#endif

    if (ratio_amount >= ratio) return 0;
    return 1;
}