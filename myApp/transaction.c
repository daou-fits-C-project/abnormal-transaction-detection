#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "transaction.h"
#include "util.h"

//#define DEBUG

void select_account_transaction(int account_id) {
	OCIStmt* stmthp;
	OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL, 
		* def5 = NULL, * def6 = NULL, * def7 = NULL, * def8 = NULL;
	OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL;

	AccountTransaction result;
	char created_at[CREATED_AT_BUF];
	int max_result = 100;

	char* sql = "SELECT * FROM (SELECT * FROM (SELECT n.account_id, n.type, (SELECT name FROM STOCK WHERE n.stock_id = stock_id) stock_name, n.amount, n.price, n.created_at, n.order_id, '정상' AS detection_type FROM normal_transaction n WHERE n.account_id = :1 UNION ALL SELECT a.transaction_id, a.type, (SELECT name FROM STOCK WHERE a.stock_id = stock_id) stock_name, a.amount, a.price, a.created_at, a.order_id, d.type_name as detection_type FROM abnormal_transaction a JOIN detection_type d ON a.detection_id = d.detection_id WHERE a.account_id = :2 ORDER BY created_at DESC) WHERE ROWNUM <= :3) ORDER BY created_at ASC";

	#ifdef DEBUG
		printf("> 새로운 핸들 생성\n");
	#endif // DEBUG
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);

	OCIDefineByPos(stmthp, &def1, errhp, 1, &result.account_id, sizeof(result.account_id), SQLT_INT, NULL, NULL,
		NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def2, errhp, 2, &result.type, sizeof(result.type), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def3, errhp, 3, result.stock_name, sizeof(result.stock_name), SQLT_STR, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def4, errhp, 4, &result.amount, sizeof(result.amount), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def5, errhp, 5, &result.price, sizeof(result.price), SQLT_FLT, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def6, errhp, 6, created_at, sizeof(created_at), SQLT_STR, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def7, errhp, 7, &result.order_id, sizeof(result.order_id), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def8, errhp, 8, result.detection_type, sizeof(result.detection_type), SQLT_STR, NULL,
		NULL, NULL, OCI_DEFAULT);

	OCIBindByPos(stmthp, &bnd1, errhp, 1, &account_id, sizeof(account_id),
		SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd2, errhp, 2, &account_id, sizeof(account_id),
		SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd3, errhp, 3, &max_result, sizeof(max_result),
		SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

	#ifdef DEBUG
		printf("> SQL문 실행 시작\n");
	#endif // DEBUG
	// SQL 실행
	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
		check_error(errhp);
	}
	else {
		#ifdef DEBUG
			printf("> SQL문 실행 성공\n");
		#endif // DEBUG
		// SQL 실행 결과
		sword status;
		char buffer[CREATED_AT_BUF];
		#ifdef DEBUG
			printf("> SQL문 결과 매핑 시작\n");
		#endif // DEBUG
			
		printf("===================================================================================================================================\n");
		printf("|                                                            조회 결과                                                            |\n");
		printf("===================================================================================================================================\n");
		printf("| 계좌 ID | 거래 유형 |      거래 종목      |    수량    |      금액      |  이상 거래 여부  |      거래 일시      | 관련 주문 ID |\n");
		printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
		while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
			== OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
			if (datetime_to_tm(created_at, &result.created_at) != 0) {
				#ifdef DEBUG
					printf("날짜 파싱 실패\n");
				#endif
				return;
			}
			if (tm_to_string(&result.created_at, buffer) != 0) {
				#ifdef DEBUG
					printf("날짜 파싱 실패\n");
				#endif
				return;
			}
			printf("| %7d | %9s | %-19s | %10d | %14.1lf | %16s | %17s | %12d |\n",
				result.account_id,
				result.type == 0 ? "매수" : "매도",
				result.stock_name,
				result.amount,
				result.price,
				result.detection_type,
				buffer,
				result.order_id
			);
			printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
		}
	}
	OCIHandleFree(stmthp, OCI_HTYPE_STMT);
	#ifdef DEBUG
		printf("> 핸들 종료\n");
	#endif // DEBUG
}

void select_stock_transaction(char* stock_id) {
	OCIStmt* stmthp;
	OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL,
		* def5 = NULL, * def6 = NULL, * def7 = NULL, * def8 = NULL;
	OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL;

	StockTransaction result;
	char created_at[CREATED_AT_BUF];
	int max_result = 100;

	char* sql = "SELECT * FROM (SELECT * FROM (SELECT (SELECT name FROM STOCK WHERE n.stock_id = stock_id) stock_name, n.type, n.account_id, n.amount, n.price, n.created_at, n.order_id, '정상' AS detection_type FROM normal_transaction n WHERE n.stock_id = :1 UNION ALL SELECT (SELECT name FROM STOCK WHERE a.stock_id = stock_id) stock_name, a.type, a.account_id, a.amount, a.price, a.created_at, a.order_id, d.type_name as detection_type FROM abnormal_transaction a JOIN detection_type d ON a.detection_id = d.detection_id WHERE a.stock_id = :2 ORDER BY created_at DESC) WHERE ROWNUM <= :3) ORDER BY created_at ASC";

#ifdef DEBUG
	printf("> 새로운 핸들 생성\n");
#endif // DEBUG
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);

	OCIDefineByPos(stmthp, &def1, errhp, 1, result.stock_name, sizeof(result.stock_name), SQLT_STR, NULL, NULL,
		NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def2, errhp, 2, &result.type, sizeof(result.type), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def3, errhp, 3, &result.account_id, sizeof(result.account_id), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def4, errhp, 4, &result.amount, sizeof(result.amount), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def5, errhp, 5, &result.price, sizeof(result.price), SQLT_FLT, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def6, errhp, 6, created_at, sizeof(created_at), SQLT_STR, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def7, errhp, 7, &result.order_id, sizeof(result.order_id), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def8, errhp, 8, result.detection_type, sizeof(result.detection_type), SQLT_STR, NULL,
		NULL, NULL, OCI_DEFAULT);

	OCIBindByPos(stmthp, &bnd1, errhp, 1, stock_id, sizeof(stock_id),
		SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd2, errhp, 2, stock_id, sizeof(stock_id),
		SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd3, errhp, 3, &max_result, sizeof(max_result),
		SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

#ifdef DEBUG
	printf("> SQL문 실행 시작\n");
#endif // DEBUG
	// SQL 실행
	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
		check_error(errhp);
	}
	else {
#ifdef DEBUG
		printf("> SQL문 실행 성공\n");
#endif // DEBUG
		// SQL 실행 결과
		sword status;
		char buffer[CREATED_AT_BUF];
#ifdef DEBUG
		printf("> SQL문 결과 매핑 시작\n");
#endif // DEBUG

		printf("===============================================================================================================================\n");
		printf("|                                                          조회 결과                                                          |\n");
		printf("===============================================================================================================================\n");
		printf("|   종목 이름   | 거래 유형 |  계좌 ID  |    수량    |      금액      |  이상 거래 여부  |      거래 일시      | 관련 주문 ID |\n");
		printf("-------------------------------------------------------------------------------------------------------------------------------\n");
		while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
			== OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
			if (datetime_to_tm(created_at, &result.created_at) != 0) {
#ifdef DEBUG
				printf("날짜 파싱 실패\n");
#endif
				return;
			}
			if (tm_to_string(&result.created_at, buffer) != 0) {
#ifdef DEBUG
				printf("날짜 파싱 실패\n");
#endif
				return;
			}
			printf("| %13s | %9s | %-9d | %10d | %14.1lf | %16s | %17s | %12d |\n",
				result.stock_name,
				result.type == 0 ? "매수" : "매도",
				result.account_id,
				result.amount,
				result.price,
				result.detection_type,
				buffer,
				result.order_id
			);
			printf("-------------------------------------------------------------------------------------------------------------------------------\n");
		}
	}
	OCIHandleFree(stmthp, OCI_HTYPE_STMT);
#ifdef DEBUG
	printf("> 핸들 종료\n");
#endif // DEBUG
}

void add_normal_transaction(StockOrder* order) {

	OCIStmt* stmthp;
	OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL, * bnd4 = NULL, * bnd5 = NULL, * bnd6 = NULL, * bnd7 = NULL;
	//printf("add_normal");
	char* sql = get_sql("sql/transaction_queries.json", "insertNormalTransaction");
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
	free(sql);


	OCIBindByPos(stmthp, &bnd1, errhp, 1, &order->price, sizeof(order->price), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd2, errhp, 2, &order->amount, sizeof(order->amount), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd3, errhp, 3, &order->type, sizeof(order->type), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd4, errhp, 4, &order->account_id, sizeof(order->account_id), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd5, errhp, 5, order->stock_id, sizeof(order->stock_id), SQLT_STR,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd6, errhp, 6, &order->order_id, sizeof(order->order_id), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
		OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
		check_error(errhp);
	}
	else {
		//printf("데이터 삽입 완료!\n");
		//print_order(order);
	}
}


void add_abnormal_transaction(StockOrder* order, int detection_id) {

	OCIStmt* stmthp;
	OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL, * bnd4 = NULL, * bnd5 = NULL, * bnd6 = NULL, * bnd7 = NULL;

	char* sql = get_sql("sql/transaction_queries.json", "insertAbnormalTransaction");
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
	free(sql);


	OCIBindByPos(stmthp, &bnd1, errhp, 1, &order->price, sizeof(order->price), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd2, errhp, 2, &order->amount, sizeof(order->amount), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd3, errhp, 3, &order->type, sizeof(order->type), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd4, errhp, 4, &detection_id, sizeof(detection_id), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd4, errhp, 5, &order->account_id, sizeof(order->account_id), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd5, errhp, 6, order->stock_id, sizeof(order->stock_id), SQLT_STR,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd6, errhp, 7, &order->order_id, sizeof(order->order_id), SQLT_INT,
		NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
		OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
		check_error(errhp);
	}
	else {
		//printf("데이터 삽입 완료!\n");
		//print_order(order);
	}
}