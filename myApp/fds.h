#pragma once

#include <time.h>

typedef enum {
    BID,  // �ż�
    ASK   // �ŵ�
} OrderType;

typedef enum {
    PENDING,  // ��ü��
    MATCHED   // ü��
} OrderStatus;

typedef struct {
    int order_id;
    double price;
    int amount;
    OrderType type;
    OrderStatus status;
    int account_id;
    char stock_id[21];
    struct tm created_at;
} StockOrder;

// FDS �Լ� (0: �̻� �ŷ�, 1: ���� �ŷ�)
int detect_stock_amount(StockOrder*);
int detect_account_amount(StockOrder*);
int detect_wash_sale(StockOrder*);
//void detect_price_volatility();
//void detect_fast_cancel();
