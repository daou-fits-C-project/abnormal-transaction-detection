#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "customer.h"
#include "db.h"

OCIStmt* stmthp;

int login(USERINFO* p_userinfo) {	// -1 : 로그인 에러, 0 : 일반 사용자, 1 : 관리자
	OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL;
	OCIBind* bnd1 = NULL, * bnd2 = NULL;
	LOGIN login;

	printf("EMAIL : ");
	scanf("%s", login.email);
	printf("PW : ");
	scanf("%s", login.password);

	char* sql = get_sql("sql/customer_queries.json", "selectByEmailPassword");
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);

	OCIBindByPos(stmthp, &bnd1, errhp, 1, login.email, sizeof(login.email),
		SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd2, errhp, 2, login.password, sizeof(login.password),
		SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def1, errhp, 1, &p_userinfo->customer_id, sizeof(p_userinfo->customer_id), SQLT_INT, NULL, NULL,
		NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def2, errhp, 2, p_userinfo->name, sizeof(p_userinfo->name), SQLT_STR, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def3, errhp, 3, &p_userinfo->is_admin, sizeof(p_userinfo->is_admin), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);

	// SQL 실행
	if (OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
		check_error(errhp);
		return LOGIN_ERROR;
	}
	else {
		// SQL 실행 결과
		sword status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT);

		free(sql);
		printf("고객 ID = %d, 고객 이름 = %s\n", p_userinfo->customer_id, p_userinfo->name);

		if (status == OCI_NO_DATA) {
			printf("해당 사용자가 존재하지 않습니다.\n");
			return LOGIN_ERROR;
		}
		else if (status == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
			printf("로그인 성공!\n");
			return p_userinfo->is_admin;
		}
		else {
			printf("예상치 못한 오류 발생.\n");
			check_error(errhp);
			return LOGIN_ERROR;
		}

	}
}

int create_user() {
	
}

int update_user() {

}

int delete_user() {

}