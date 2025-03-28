#pragma once
#include <time.h>
#define STOCK_ID_BUF 32
#define ORDER_TOKEN 5
#define STATUS_BUF 32

typedef enum {
    BID,  // 매수
    ASK   // 매도
} OrderType;

typedef enum {
    PENDING,  // 미체결
    MATCHED,  // 체결
    CANCELED  // 취소
} OrderStatus;

typedef struct {
    int order_id;
    char stock_id[STOCK_ID_BUF];
    int price;
    int amount;
    int account_id;
    OrderType type;
    OrderStatus status;
    struct tm created_at;
} StockOrder;


// 주요 함수 선언
void add_order(const char* csv);
void print_orders(StockOrder* order_arr, int count);

int get_order_count_by_account(int account_id);
void get_orders_by_account(StockOrder* order_arr, int account_id);
void handle_order();

// 직렬화 및 파싱 관련
void order_to_csv_string(const StockOrder* order, char* buffer, size_t bufsize);
int csv_string_to_order(const char* csv, StockOrder* order);

// ENUM ↔ 문자열 변환
const char* order_status_to_string(OrderStatus status);
const char* order_type_to_string(OrderType type);
OrderStatus parse_order_status(const char* str);
OrderType parse_order_type(const char* str);