#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "db.h"

void select_account_transaction() {
//	OCIStmt* stmthp;
//	OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL;
//	int cnt;
//	#ifdef DEBUG
//		printf("> ��� �� ��ȸ ����\n");
//	#endif // DEBUG
//
//	// ��� �� ��ȸ
//	char* sql = get_sql("sql/customer_queries.json", "selectWithAccountsCnt");
//
//#ifdef DEBUG
//	printf("> DB �ڵ� ����\n");
//#endif // DEBUG
//
//	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
//	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
//	OCIDefineByPos(stmthp, &def1, errhp, 1, &cnt, sizeof(cnt), SQLT_INT, NULL, NULL,
//		NULL, OCI_DEFAULT);
//#ifdef DEBUG
//	printf("> SQL��(selectWithAccountsCnt) ���� ����\n");
//#endif // DEBUG
//	// SQL ����
//	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
//		check_error(errhp);
//	}
//	else {
//#ifdef DEBUG
//		printf("> SQL��(selectWithAccountsCnt) ���� ����\n");
//#endif // DEBUG
//		// SQL ���� ���
//		sword status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT);
//#ifdef DEBUG
//		printf("> ��ȸ ���: ");
//		if (status == OCI_NO_DATA) {
//			printf("�����Ͱ� �������� �ʽ��ϴ�.\n");
//		}
//		else if (status == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
//			printf("> �� %d���� ���\n", cnt);
//		}
//		else {
//			printf("> ����ġ ���� ���� �߻� : \n");
//			check_error(errhp);
//		}
//#endif // DEBUG
//
//	}
//
//	free(sql);
//
//#ifdef DEBUG
//	printf("> SQL��(selectWithAccountsCnt) ���� �Ϸ�\n");
//#endif // DEBUG
//
//	if (cnt > 0) {
//#ifdef DEBUG
//		printf("> SQL��(selectWithAccounts) ���� �غ�\n");
//#endif // DEBUG
//		UserAccInfo result;
//
//		char* sqlAccount = "select c.customer_id, c.name, a.account_id, a.status from customer c, account a where c.customer_id = a.customer_id order by a.status DESC, c.customer_id";
//
//#ifdef DEBUG
//		printf("> ���ο� �ڵ� ����\n");
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
//		printf("> SQL��(selectWithAccounts) ���� ����\n");
//#endif // DEBUG
//		// SQL ����
//		if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
//			check_error(errhp);
//		}
//		else {
//#ifdef DEBUG
//			printf("> SQL��(selectWithAccounts) ���� ����\n");
//#endif // DEBUG
//			// SQL ���� ���
//			sword status;
//			char* acc_status_to_str[ACC_STATUS_LEN] = { "ACTIVE", "SUSPENDED", "LOCKED", "CLOSED", "PENDING", "FROZEN", "UNVERIFIED", "RESTRICTED" };
//#ifdef DEBUG
//			printf("> SQL��(selectWithAccounts) ��� ���� ����\n");
//#endif // DEBUG
//
//			printf("=====================================================\n");
//			printf("|                     ��ȸ ���                     |\n");
//			printf("=====================================================\n");
//			printf("| �� ID |     �̸�     | ���� ��ȣ |     ����     |\n");
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
//	printf("> �ڵ� ����\n");
//#endif // DEBUG
}

void select_stock_transaction() {

}