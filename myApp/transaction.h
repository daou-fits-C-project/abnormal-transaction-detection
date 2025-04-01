#pragma once
#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <time.h>
#include "order.h"
#define DETECTION_TYPE_BUF 100
#define STOCK_NAME_BUF 100
#define MAX_RESULT_CNT 100
#define TYPE_NAME_BUF 24
#define DESCRIPTION_BUF 200

typedef struct {
	int transaction_id;
	int price;
	int amount;
	int type;
	int account_id;
	char stock_id[STOCK_ID_BUF];
	int order_id;
	struct tm created_at;
} Transaction;

//typedef struct {
//	int order_id;
//	int account_id;
//	char stock_id[STOCK_ID_BUF];
//	int price;
//	int amount;
//	int type;
//	char type_name[TYPE_NAME_BUF];
//	char description[DESCRIPTION_BUF];
//	struct tm created_at;
//} AbnormalTransaction;

typedef struct {
	int account_id;
	int type;
	int amount;
	int order_id;
	double price;
	char stock_name[STOCK_NAME_BUF];
	char detection_type[DETECTION_TYPE_BUF];
	struct tm created_at;
} AccountTransaction;

typedef struct {
	char stock_name[STOCK_NAME_BUF];
	int type;
	int amount;
	int account_id;
	int order_id;
	double price;
	char detection_type[DETECTION_TYPE_BUF];
	struct tm created_at;
} StockTransaction;

void select_account_transaction(int account_id);
void select_stock_transaction(char* stock_id);
void add_normal_transaction(StockOrder* order);
void add_abnormal_transaction(StockOrder* order, int detection_id);
void handle_transaction_log();
#endif