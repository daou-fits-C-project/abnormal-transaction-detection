#include <stdio.h>
#include <time.h>
#include <string.h>
#include "util.h"
int datetime_to_tm(const char* datetime_str, struct tm* result) {
    if (!datetime_str || !result) return -1;

    int y, m, d, h, min, s;
    int parsed = sscanf(datetime_str, "%d/%d/%d %d:%d:%d", &y, &m, &d, &h, &min, &s);
    if (parsed != 6) return -1;
    if (y < 100) y += 2000;

    //memset(result, 0, sizeof(struct tm));
    result->tm_year = y - 1900;
    result->tm_mon = m - 1;
    result->tm_mday = d;
    result->tm_hour = h;
    result->tm_min = min;
    result->tm_sec = s;

    return 0;
}

int tm_to_string(const struct tm* t, char* buffer) {

    size_t len = strftime(buffer, CREATED_AT_BUF, "%Y/%m/%d %H:%M:%S", t);
    if (len == 0) {
        fprintf(stderr, "[ERROR] tm_to_string: 포맷 실패 or 버퍼 부족\n");
        return -1;
    }

    return 0;
}