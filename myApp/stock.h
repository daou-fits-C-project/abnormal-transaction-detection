#pragma once

typedef enum {
    STOCK_ACTIVE,            // 정상 사용 중
    STOCK_SUSPENDED,         // 일시 정지 (고객 요청 or 이상거래 감지)
} StockStatus;


typedef struct {
	char stock_id[32];
	char stock_name[20];
	int price;
	StockStatus status;

} Stock;

void get_stock_status(Stock* stock_ptr, char* stock_id);
void update_stock_status(char* stock_id, StockStatus status);