#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "db.h"
#include "stock.h"
void get_stock_status(Stock *stock_ptr, char *stock_id) {
    OCIStmt* stmthp;
    sword status;
    OCIBind* bnd1 = NULL;
    OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, * def5 = NULL;
    StockStatus stock_status;
    // 1.  SQL문 핸들을 생성
    char* sql = get_sql("sql/stock_queries.json", "getStatusByStockId");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX,
        OCI_DEFAULT);
    free(sql);

    OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_id, sizeof(stock_id), SQLT_STR,
        NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    // 2. 쿼리 실행
    OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

    // 3. 데이터 바인딩 (결과를 받을 변수)
    OCIDefineByPos(stmthp, &def1, errhp, 1, &stock_status, sizeof(stock_status), SQLT_INT, NULL, NULL,
        NULL, OCI_DEFAULT);


    // 4. 데이터 가져오기 
    if ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
        == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        stock_ptr->status = stock_status;

    }

    OCIHandleFree(stmthp, OCI_HTYPE_STMT);
}


void update_stock_status(char *stock_id, StockStatus status) {
    OCIStmt* stmthp;
    OCIBind* bnd1 = NULL, * bnd2 = NULL;

    char* sql = get_sql("sql/stock_queries.json", "updateStatusById");
    OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
    OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql),
        OCI_NTV_SYNTAX, OCI_DEFAULT);
    free(sql);


    // 바인딩 변수 설정 (UPDATE)
    OCIBindByPos(stmthp, &bnd1, errhp, 1, &status, sizeof(status),
        SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bnd2, errhp, 2, stock_id, sizeof(stock_id), SQLT_STR,
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
