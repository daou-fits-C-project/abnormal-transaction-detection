#pragma once
#include "socket.h"


#define QUEUE_SIZE 1024

typedef struct {
    SOCKET client_socket;
    char data[1024];
} OrderTask;

OrderTask queue[QUEUE_SIZE];
extern int head;
extern int tail;
CRITICAL_SECTION queue_lock;
HANDLE queue_not_empty_event;


void enqueue(OrderTask task);
OrderTask dequeue();