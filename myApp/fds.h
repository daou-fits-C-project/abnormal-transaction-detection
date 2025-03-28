#pragma once

#include <time.h>
#include "order.h"

typedef struct {
	int order_id;
	int account_id;
	char stock_id[21];
	int price;
	int amount;
	int type;
	char type_name[51];
	char description[201];
	struct tm created_at;
} AbnormalTransaction;

// FDS 함수 (0: 이상 거래, 1: 정상 거래)
int detect_stock_amount(StockOrder*);
int detect_account_amount(StockOrder*);
int detect_wash_sale(StockOrder*);
void report_FDS();
