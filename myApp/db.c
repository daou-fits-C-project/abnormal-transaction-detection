#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "cJSON.h"

OCIEnv* envhp = NULL;
OCIError* errhp = NULL;
OCISvcCtx* svchp = NULL;
static OCISession* usrhp = NULL;
static OCIServer* srvhp = NULL;

int db_init() {
    char* username = "C##DEV";
    char* password = "1234";
    char* dbname = "192.168.31.72:1521/xe";

    // 환경 핸들 생성
    if (OCIEnvCreate(&envhp, OCI_DEFAULT, NULL, NULL, NULL, NULL, 0, NULL) !=
        OCI_SUCCESS) {
        printf("OCIEnvCreate failed\n");
        return -1;
    }
    // 오류 핸들 생성
    if (OCIHandleAlloc((dvoid*)envhp, (dvoid**)&errhp, OCI_HTYPE_ERROR, (size_t)0,
        (dvoid**)NULL) != OCI_SUCCESS) {
        printf("OCIHandleAlloc failed for error handle\n");
        return -1;
    }
    // 서버 핸들 생성
    if (OCIHandleAlloc((dvoid*)envhp, (dvoid**)&srvhp, OCI_HTYPE_SERVER, (size_t)0,
        (dvoid**)NULL) != OCI_SUCCESS) {
        printf("OCIHandleAlloc failed for server handle\n");
        return -1;
    }
    // 서버 연결
    if (OCIServerAttach(srvhp, errhp, (text*)dbname, strlen(dbname), OCI_DEFAULT) !=
        OCI_SUCCESS) {
        check_error(errhp);
        return -1;
    }
    // 서비스 컨텍스트 생성
    if (OCIHandleAlloc((dvoid*)envhp, (dvoid**)&svchp, OCI_HTYPE_SVCCTX, (size_t)0,
        (dvoid**)NULL) != OCI_SUCCESS) {
        check_error(errhp);
        return -1;
    }
    // 세션 핸들 생성 및 연결
    if (OCIHandleAlloc((dvoid*)envhp, (dvoid**)&usrhp, OCI_HTYPE_SESSION, (size_t)0,
        (dvoid**)NULL) != OCI_SUCCESS) {
        check_error(errhp);
        return -1;
    }
    if (OCILogon2(envhp, errhp, &svchp,
        (OraText*)username, (ub4)strlen(username),
        (OraText*)password, (ub4)strlen(password),
        (OraText*)dbname, (ub4)strlen(dbname),
        OCI_DEFAULT /* 마지막 인수 추가 */
    ) != OCI_SUCCESS) {
        check_error(errhp);
        return -1;
    }
    printf("Oracle Database connected successfully.\n");
    return 0;
}


void db_cleanup() {
    OCILogoff(svchp, errhp);
    OCIHandleFree((dvoid*)usrhp, OCI_HTYPE_SESSION);
    OCIHandleFree((dvoid*)svchp, OCI_HTYPE_SVCCTX);
    OCIHandleFree((dvoid*)srvhp, OCI_HTYPE_SERVER);
    OCIHandleFree((dvoid*)errhp, OCI_HTYPE_ERROR);
    OCIHandleFree((dvoid*)envhp, OCI_HTYPE_ENV);
    printf("Oracle Database disconnected successfully.\n");
}


void check_error(OCIError* errhp) {
    text errbuf[512];
    sb4 errcode = 0;
    OCIErrorGet((dvoid*)errhp, (ub4)1, (text*)NULL, &errcode, errbuf, (ub4)sizeof(errbuf),
        OCI_HTYPE_ERROR);
    printf("Error: %s\n", errbuf);
}

cJSON* load_json(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("파일 열기 실패");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }

    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);

    cJSON* root = cJSON_Parse(buffer);
    free(buffer);

    if (!root) {
        fprintf(stderr, "JSON 파싱 실패: %s\n", cJSON_GetErrorPtr());
    }

    return root;
}

char* get_sql(const char* filename, const char* mapper) {

    cJSON* root = load_json(filename);
    if (!root)
    {
        printf("json load 실패\n");
        return NULL;
    }
    char* buffer = malloc(MAX_QUERY_SIZE);
    strcpy(buffer, cJSON_GetObjectItem(root, mapper)->valuestring);
    cJSON_Delete(root);
    return buffer;
}

