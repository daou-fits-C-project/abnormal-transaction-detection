#pragma once
#ifndef MANAGE_H
#define MANAGE_H
#include "customer.h"

extern UserInfo current_user;

void run_manage();
void handle_transaction_log();
void handle_order();
void abnormal_analysis_report();
#endif