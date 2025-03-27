#pragma once
#ifndef CUSTOMER_H
#define CUSTOMER_H
#define MAX_EMAIL_LEN 50
#define MAX_PW_LEN 50
#define MAX_NAME_LEN 50
#define ADMIN 1
#define NORMAL 0
#define LOGIN_ERROR -1

typedef struct {
	char email[MAX_EMAIL_LEN];
	char password[MAX_PW_LEN];
} LOGIN;

typedef struct {
	int customer_id;
	char name[MAX_NAME_LEN];
	int is_admin;
} USERINFO;

int login(USERINFO* p_userinfo);	// ·Î±×ÀÎ

#endif