#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "customer.h"
#include "db.h"

OCIStmt* stmthp;

extern UserInfo current_user;

void login() {
	OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL;
	OCIBind* bnd1 = NULL, * bnd2 = NULL;
	Login login;
	int is_error = LOGIN_ERROR;
	
	char* sql = get_sql("sql/customer_queries.json", "selectByEmailPassword");
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
	
	OCIDefineByPos(stmthp, &def1, errhp, 1, &current_user.customer_id, sizeof(current_user.customer_id), SQLT_INT, NULL, NULL,
		NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def2, errhp, 2, current_user.name, sizeof(current_user.name), SQLT_STR, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def3, errhp, 3, &current_user.is_admin, sizeof(current_user.is_admin), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);

	while (is_error == LOGIN_ERROR || current_user.is_admin != ADMIN) {
		printf("EMAIL : ");
		scanf("%s", login.email);
		printf("PW : ");
		scanf("%s", login.password);

		OCIBindByPos(stmthp, &bnd1, errhp, 1, login.email, sizeof(login.email),
			SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
		OCIBindByPos(stmthp, &bnd2, errhp, 2, login.password, sizeof(login.password),
			SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

		// SQL 실행
		if (OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
			printf("SQL 실행 중 오류가 발생했습니다.");
			check_error(errhp);
		}
		else {
			// SQL 실행 결과
			sword status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT);
			system("cls");
			if (status == OCI_NO_DATA) {
				printf("해당 사용자가 존재하지 않습니다.\n");
			}
			else if (status == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
				printf("%s님 환영합니다.\n", current_user.name);
				is_error = LOGIN_SUCCESS;
			}
			else {
				printf("예상치 못한 오류 발생.\n");
				check_error(errhp);
			}

		}
	}

	free(sql);
}

void select_user() {	// 0 : 일반 사용자, 1 : 관리자
	OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL;
	OCIBind* bnd1 = NULL;
	UserInfo selected_user;
	char email[MAX_EMAIL_LEN];

	char* sql = get_sql("sql/customer_queries.json", "selectByEmailPassword");
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);

	OCIDefineByPos(stmthp, &def1, errhp, 1, &selected_user.customer_id, sizeof(selected_user.customer_id), SQLT_INT, NULL, NULL,
		NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def2, errhp, 2, selected_user.name, sizeof(selected_user.name), SQLT_STR, NULL,
		NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def3, errhp, 3, &selected_user.is_admin, sizeof(selected_user.is_admin), SQLT_INT, NULL,
		NULL, NULL, OCI_DEFAULT);

	printf("EMAIL : ");
	scanf("%s", email);

	OCIBindByPos(stmthp, &bnd1, errhp, 1, email, sizeof(email),
		SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

	// SQL 실행
	if (OCIStmtExecute(svchp, stmthp, errhp, 0, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
		check_error(errhp);
	}
	else {
		// SQL 실행 결과
		sword status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT);

		printf("조회 결과: ");
		if (status == OCI_NO_DATA) {
			printf("해당 사용자가 존재하지 않습니다.\n");
		}
		else if (status == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
			printf("\n고객 ID = %d, 고객 이름 = %s\n", selected_user.customer_id, selected_user.name);
		}
		else {
			printf("예상치 못한 오류 발생.\n");
			check_error(errhp);
		}

	}

	free(sql);
}

void create_user() {
	OCIBind* bnd1 = NULL, * bnd2 = NULL, * bnd3 = NULL, * bnd4 = NULL;
	SignUp signup;

	printf("[회원 가입]\n");
	printf("EMAIL : ");
	scanf("%s", signup.email);
	printf("PW : ");
	scanf("%s", signup.password);
	printf("NAME : ");
	scanf("%s", signup.name);
	printf("IS_ADMIN : ");
	scanf("%d", &signup.is_admin);

	char* sql = get_sql("sql/customer_queries.json", "insert");
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);

	OCIBindByPos(stmthp, &bnd1, errhp, 1, signup.email, sizeof(signup.email),
		SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd2, errhp, 2, signup.password, sizeof(signup.password),
		SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd3, errhp, 3, signup.name, sizeof(signup.name),
		SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmthp, &bnd4, errhp, 4, &signup.is_admin, sizeof(signup.is_admin),
		SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
		OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
		printf("회원 가입에 실패하였습니다.\n");
		check_error(errhp);
	}
	else {
		free(sql);
	}
}

void update_user() {
	OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL;
	OCIBind* bnd1 = NULL;
	SignUp update;
	int choice, is_malloc = 0;
	char* sql = NULL;
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);

	while (1) {
		printf("[회원 정보 수정]\n");
		printf("1. 이름 수정\n");
		printf("2. 비밀번호 수정\n");
		printf("3. 권한 수정\n");
		printf("4. 종료\n");
		printf("----------------\n");
		printf("선택 : ");
		scanf("%d", &choice);

		switch (choice) {
		case 1:
			printf("변경할 이름을 입력하세요 : ");
			scanf("%s", update.name);
			sql = get_sql("sql/customer_queries.json", "updateName");
			is_malloc = 1;
			OCIBindByPos(stmthp, &bnd1, errhp, 1, update.name, sizeof(update.name),
				SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
			break;
		case 2:
			printf("변경할 패스워드를 입력하세요 : ");
			scanf("%s", update.password);
			sql = get_sql("sql/customer_queries.json", "updatePassword");
			is_malloc = 1;
			OCIBindByPos(stmthp, &bnd1, errhp, 1, update.password, sizeof(update.password),
				SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
			break;
		case 3:
			printf("변경할 권한을 입력해주세요 (0: 일반 사용자, 1: 관리자) : ");
			scanf("%d", &update.is_admin);
			sql = get_sql("sql/customer_queries.json", "updateIsAdmin");
			is_malloc = 1;
			OCIBindByPos(stmthp, &bnd1, errhp, 1, &update.is_admin, sizeof(update.is_admin),
				SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
			break;
		case 4:
			if (is_malloc == 1) {
				free(sql);
			}
			return;
		default:
			printf("올바른 메뉴를 선택해주세요.\n");
			break;
		}
	}
	
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);

	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
		OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
		printf("회원 정보 수정에 실패하였습니다.\n");
		check_error(errhp);
	}
	else {
		printf("회원 정보 수정 성공!\n");
	}

}

void delete_user() {
	OCIBind* bnd1 = NULL;
	char email[MAX_EMAIL_LEN];

	printf("탈퇴할 고객의 이메일을 입력하세요 : ");
	scanf("%s", email);

	char* sql = get_sql("sql/customer_queries.json", "updateName");
	OCIBindByPos(stmthp, &bnd1, errhp, 1, email, sizeof(email),
		SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	
	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);

	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
		OCI_COMMIT_ON_SUCCESS) != OCI_SUCCESS) {
		printf("회원 탈퇴에 실패하였습니다.\n");
		check_error(errhp);
	}
	else {
		printf("회원 탈퇴 성공!\n");
	}
}

void select_all_with_accounts() {
	OCIStmt* stmthp;
	OCIDefine* def1 = NULL, * def2 = NULL, * def3 = NULL, * def4 = NULL;
	int cnt;
	#ifdef DEBUG
		printf("> 총 개수 조회 시작\n");
	#endif // DEBUG

	// 총 개수 조회
	char* sql = get_sql("sql/customer_queries.json", "selectWithAccountsCnt");

	#ifdef DEBUG
		printf("> DB 핸들 설정\n");
	#endif // DEBUG

	OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
	OCIStmtPrepare(stmthp, errhp, (text*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
	OCIDefineByPos(stmthp, &def1, errhp, 1, &cnt, sizeof(cnt), SQLT_INT, NULL, NULL,
		NULL, OCI_DEFAULT);
	#ifdef DEBUG
		printf("> SQL문(selectWithAccountsCnt) 실행 시작\n");
	#endif // DEBUG
	// SQL 실행
	if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
		check_error(errhp);
	}
	else {
		#ifdef DEBUG
				printf("> SQL문(selectWithAccountsCnt) 실행 성공\n");
		#endif // DEBUG
		// SQL 실행 결과
		sword status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT);
		#ifdef DEBUG
			printf("> 조회 결과: ");
			if (status == OCI_NO_DATA) {
				printf("데이터가 존재하지 않습니다.\n");
			}
			else if (status == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
				printf("> 총 %d개의 결과\n", cnt);
			}
			else {
				printf("> 예상치 못한 오류 발생 : \n");
				check_error(errhp);
			}
		#endif // DEBUG

	}
	
	free(sql);

	#ifdef DEBUG
		printf("> SQL문(selectWithAccountsCnt) 실행 완료\n");
	#endif // DEBUG
	
	if (cnt > 0) {
		#ifdef DEBUG
				printf("> SQL문(selectWithAccounts) 실행 준비\n");
		#endif // DEBUG
		UserAccInfo result;
		
		char* sqlAccount = "select c.customer_id, c.name, a.account_id, a.status from customer c, account a where c.customer_id = a.customer_id order by a.status DESC, c.customer_id";
		
		#ifdef DEBUG
				printf("> 새로운 핸들 생성\n");
		#endif // DEBUG
		OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, NULL);
		OCIStmtPrepare(stmthp, errhp, (text*)sqlAccount, strlen(sqlAccount), OCI_NTV_SYNTAX, OCI_DEFAULT);

		OCIDefineByPos(stmthp, &def1, errhp, 1, &result.customer_id, sizeof(result.customer_id), SQLT_INT, NULL, NULL,
			NULL, OCI_DEFAULT);
		OCIDefineByPos(stmthp, &def2, errhp, 2, result.name, sizeof(result.name), SQLT_STR, NULL,
			NULL, NULL, OCI_DEFAULT);
		OCIDefineByPos(stmthp, &def3, errhp, 3, &result.account_id, sizeof(result.account_id), SQLT_INT, NULL,
			NULL, NULL, OCI_DEFAULT);
		OCIDefineByPos(stmthp, &def4, errhp, 4, &result.status, sizeof(result.status), SQLT_INT, NULL,
			NULL, NULL, OCI_DEFAULT);
		#ifdef DEBUG
				printf("> SQL문(selectWithAccounts) 실행 시작\n");
		#endif // DEBUG
		// SQL 실행
		if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT) != OCI_SUCCESS) {
			check_error(errhp);
		}
		else {
			#ifdef DEBUG
						printf("> SQL문(selectWithAccounts) 실행 성공\n");
			#endif // DEBUG
			// SQL 실행 결과
			sword status;
			char* acc_status_to_str[ACC_STATUS_LEN] = { "ACTIVE", "SUSPENDED", "LOCKED", "CLOSED", "PENDING", "FROZEN", "UNVERIFIED", "RESTRICTED" };
			#ifdef DEBUG
						printf("> SQL문(selectWithAccounts) 결과 매핑 시작\n");
			#endif // DEBUG
			
			printf("=====================================================\n");
			printf("|                     조회 결과                     |\n");
			printf("=====================================================\n");
			printf("| 고객 ID |     이름     | 계좌 번호 |     상태     |\n");
			printf("-----------------------------------------------------\n");
			while ((status = OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 0, OCI_DEFAULT))
				== OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
				printf("| %7d | %12s | %-9d | %-12s |\n",
					result.customer_id,
					result.name,
					result.account_id,
					acc_status_to_str[result.status]
				);
			}
			printf("-----------------------------------------------------\n");
		}
	}
	OCIHandleFree(stmthp, OCI_HTYPE_STMT);
	#ifdef DEBUG
		printf("> 핸들 종료\n");
	#endif // DEBUG
}