#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "db.h"

void select_account_transaction() {
//	OCIStmt* stmthp;
//	OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL;
//	int cnt;
//	#ifdef DEBUG
//		printf("> 결과 수 조회 시작\n");
//	#endif // DEBUG
//
//	// 결과 수 조회
//	char* sql = get_sql("sql/customer_queries.json", "selectWithAccountsCnt");
//
//#ifdef DEBUG
//	printf("> DB 핸들 설정\n");
//#endif // DEBUG
//
//	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
//	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
//	OCIDefineByPos(stmthp, &def1, errhp, 1, &cnt, sizeof(cnt), SQLT_INT, NULL, NULL,
//		NULL, OCI_DEFAULT);
//#ifdef DEBUG
//	printf("> SQL문(selectWithAccountsCnt) 실행 시작\n");
//#endif // DEBUG
//	// SQL 실행
//	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
//		check_error(errhp);
//	}
//	else {
//#ifdef DEBUG
//		printf("> SQL문(selectWithAccountsCnt) 실행 성공\n");
//#endif // DEBUG
//		// SQL 실행 결과
//		sword status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT);
//#ifdef DEBUG
//		printf("> 조회 결과: ");
//		if (status == OCI_NO_DATA) {
//			printf("데이터가 존재하지 않습니다.\n");
//		}
//		else if (status == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
//			printf("> 총 %d개의 결과\n", cnt);
//		}
//		else {
//			printf("> 예상치 못한 오류 발생 : \n");
//			check_error(errhp);
//		}
//#endif // DEBUG
//
//	}
//
//	free(sql);
//
//#ifdef DEBUG
//	printf("> SQL문(selectWithAccountsCnt) 실행 완료\n");
//#endif // DEBUG
//
//	if (cnt > 0) {
//#ifdef DEBUG
//		printf("> SQL문(selectWithAccounts) 실행 준비\n");
//#endif // DEBUG
//		UserAccInfo result;
//
//		char* sqlAccount = "select c.customer_id, c.name, a.account_id, a.status from customer c, account a where c.customer_id = a.customer_id order by a.status DESC, c.customer_id";
//
//#ifdef DEBUG
//		printf("> 새로운 핸들 생성\n");
//#endif // DEBUG
//		OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
//		OCIStmtPrepare(stmthp, errhp, (text*)sqlAccount, strlen(sqlAccount), OCI_NTV_SYNTAX, OCI_DEFAULT);
//
//		OCIDefineByPos(stmthp, &def1, errhp, 1, &result.customer_id, sizeof(result.customer_id), SQLT_INT, NULL, NULL,
//			NULL, OCI_DEFAULT);
//		OCIDefineByPos(stmthp, &def2, errhp, 2, result.name, sizeof(result.name), SQLT_STR, NULL,
//			NULL, NULL, OCI_DEFAULT);
//		OCIDefineByPos(stmthp, &def3, errhp, 3, &result.account_id, sizeof(result.account_id), SQLT_INT, NULL,
//			NULL, NULL, OCI_DEFAULT);
//		OCIDefineByPos(stmthp, &def4, errhp, 4, &result.status, sizeof(result.status), SQLT_INT, NULL,
//			NULL, NULL, OCI_DEFAULT);
//#ifdef DEBUG
//		printf("> SQL문(selectWithAccounts) 실행 시작\n");
//#endif // DEBUG
//		// SQL 실행
//		if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
//			check_error(errhp);
//		}
//		else {
//#ifdef DEBUG
//			printf("> SQL문(selectWithAccounts) 실행 성공\n");
//#endif // DEBUG
//			// SQL 실행 결과
//			sword status;
//			char* acc_status_to_str[ACC_STATUS_LEN] = { "ACTIVE", "SUSPENDED", "LOCKED", "CLOSED", "PENDING", "FROZEN", "UNVERIFIED", "RESTRICTED" };
//#ifdef DEBUG
//			printf("> SQL문(selectWithAccounts) 결과 매핑 시작\n");
//#endif // DEBUG
//
//			printf("=====================================================\n");
//			printf("|                     조회 결과                     |\n");
//			printf("=====================================================\n");
//			printf("| 고객 ID |     이름     | 계좌 번호 |     상태     |\n");
//			printf("-----------------------------------------------------\n");
//			while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
//				== OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
//				printf("| %7d | %12s | %-9d | %-12s |\n",
//					result.customer_id,
//					result.name,
//					result.account_id,
//					acc_status_to_str[result.status]
//				);
//			}
//			printf("-----------------------------------------------------\n");
//		}
//	}
//	OCIHandleFree(stmthp, OCI_HTYPE_STMT);
//#ifdef DEBUG
//	printf("> 핸들 종료\n");
//#endif // DEBUG
}

void select_stock_transaction() {

}