#include "order_queue.h"

int tail = 0;
int head = 0;

void enqueue(OrderTask task) {
    EnterCriticalSection(&queue_lock);
    queue[tail] = task;
    tail = (tail + 1) % QUEUE_SIZE;
    LeaveCriticalSection(&queue_lock);
    SetEvent(queue_not_empty_event); // 워커 스레드 깨움
}

OrderTask dequeue() {
    OrderTask task;
    EnterCriticalSection(&queue_lock);
    task = queue[head];
    head = (head + 1) % QUEUE_SIZE;
    LeaveCriticalSection(&queue_lock);
    return task;
}