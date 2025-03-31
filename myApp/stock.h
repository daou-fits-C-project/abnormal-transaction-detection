#pragma once

typedef enum {
    STOCK_ACTIVE,            // ���� ��� ��
    STOCK_SUSPENDED,         // �Ͻ� ���� (�� ��û or �̻�ŷ� ����)
} StockStatus;


typedef struct {
	char stock_id[32];
	char stock_name[20];
	int price;
	StockStatus status;

} Stock;

void get_stock_status(Stock* stock_ptr, char* stock_id);
void update_stock_status(char* stock_id, StockStatus status);