#pragma once

#define CREATED_AT_BUF 64

int datetime_to_tm(const char* datetime_str, struct tm* result);
int tm_to_string(const struct tm* t, char* buffer);