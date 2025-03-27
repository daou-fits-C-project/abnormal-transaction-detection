#pragma once
#ifndef USER_H
#define USER_H
#define MAX_EMAIL_LEN 50
#define MAX_PW_LEN 50
#define MAX_NAME_LEN 50
typedef struct {
	char email[MAX_EMAIL_LEN];
	char password[MAX_PW_LEN];
} LOGIN;

typedef struct {
	int customer_id;
	char name[MAX_NAME_LEN];
} USERINFO;

void login(USERINFO* p_userinfo);	// ·Î±×ÀÎ

#endif