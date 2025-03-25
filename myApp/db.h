#pragma once
#include <oci.h>
#define MAX_QUERY_SIZE 100

int db_init();
void db_cleanup();
void check_error(OCIError* errhp);
char* get_sql(const char* filename, const char* mapper);

extern OCIEnv* envhp;
extern OCIError* errhp;
extern OCISvcCtx* svchp;