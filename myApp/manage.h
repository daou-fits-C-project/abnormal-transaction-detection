#pragma once
#ifndef MANAGE_H
#define MANAGE_H
#include "customer.h"

extern UserInfo current_user;
extern volatile int g_stopMonitoring;
void run_manage();
void handle_transaction_log();
void abnormal_analysis_report();
void to_be_continue();
unsigned __stdcall real_time_monitoring(void* arg);
#endif