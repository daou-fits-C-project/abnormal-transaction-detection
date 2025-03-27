#pragma once

#include <time.h>

typedef enum {
    BID,  // 매수
    ASK   // 매도
} OrderType;

typedef enum {
    MATCHED,  // 체결
    PENDING   // 미체결
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

// FDS 함수 (0: 이상 거래, 1: 정상 거래)
int detect_stock_amount(StockOrder*);
int detect_account_amount(StockOrder*);
//int detect_repeat_trades(StockOrder*);
//void detect_price_volatility();
//void detect_fast_cancel();
