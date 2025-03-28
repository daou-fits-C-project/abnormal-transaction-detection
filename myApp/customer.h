#pragma once
#ifndef CUSTOMER_H
#define CUSTOMER_H
#define MAX_EMAIL_LEN 50
#define MAX_PW_LEN 50
#define MAX_NAME_LEN 50
#define LOGIN_SUCCESS 1
#define LOGIN_ERROR 0
#define ACC_STATUS_LEN 8
#include "account.h"

typedef enum { NORMAL, ADMIN } UserType;

typedef struct {
	char email[MAX_EMAIL_LEN];
	char password[MAX_PW_LEN];
} Login;

typedef struct {
	int customer_id;
	char name[MAX_NAME_LEN];
	UserType is_admin;
} UserInfo;

typedef struct {
	char email[MAX_EMAIL_LEN];
	char password[MAX_PW_LEN];
	char name[MAX_NAME_LEN];
	UserType is_admin;
} SignUp;

typedef struct {
	int customer_id;
	char name[MAX_NAME_LEN];
	int account_id;
	AccountStatus status;
} UserAccInfo;

UserInfo current_user;

void login();	// ·Î±×ÀÎ
void select_user();
void create_user();
void update_user();
void delete_user();
void select_all_with_accounts();
#endif