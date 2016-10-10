#include "response.h"
#include <fcgiapp.h>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#define _strdup(a) strdup(a)
#endif

void cw_res_init(cw_response *res)
{
    memset(res, 0, sizeof(cw_response));
}

void cw_res_free(cw_response *res) {
    int i;
    for (i=0;i!=MAXHEADERS*2;++i) {
        free(res->headers[i]);
    }
    for (i=0;i!=MAXCOOKIES*2;++i) {
        free(res->cookies[i]);
    }
    free(res->status);
}

void cw_res_set_status(cw_response *res, int http_code, const char *message)
{
    res->http_code = http_code;
    free(res->status);
    res->status = _strdup(message);
}

int cw_res_set_header(cw_response *res, const char *name, const char *value)
{
    int i;
    for (i=0;i!=MAXHEADERS;++i) {
        if (res->headers[i*2]==NULL || strcmp(res->headers[i*2], name)==0) {
            res->headers[i*2] = _strdup(name);
            res->headers[i*2+1] = _strdup(value);
            return 0;
        }
    }
    return -1;
}

int cw_res_set_cookie(cw_response *res, const char *name, const char *value)
{
    int i;
    for (i=0;i!=MAXCOOKIES;++i) {
        if (res->cookies[i*2]==NULL || strcmp(res->cookies[i*2], name)==0) {
            res->cookies[i*2] = _strdup(name);
            res->cookies[i*2+1] = _strdup(value);
            return 0;
        }
    }
    return -1;
}

void cw_res_print_headers(cw_response *res, FCGX_Stream *out)
{
    int i;
    
    FCGX_FPrintF(out,
                 "Status: %d %s\r\n",
                 res->http_code ? res->http_code : 200,
                 res->status ? res->status : "Okay");
    for (i=0;i!=MAXHEADERS;++i) {
        char * name = res->headers[i*2];
        if (name) {
            char * value = res->headers[i*2+1];
            FCGX_FPrintF(out, "%s: %s\r\n", name, value);
        } else {
            break;
        }
    }
    if (res->cookies[0]) {
        FCGX_FPrintF(out, "Set-Cookie: ");
        for (i=0;i!=MAXCOOKIES;++i) {
            char * name = res->cookies[i*2];
            if (name) {
                char * value = res->cookies[i*2+1];
                if (i>0) {
                    FCGX_FPrintF(out, ",", name, value);
                }
                FCGX_FPrintF(out, "%s=%s", name, value);
            }
        }
        FCGX_FPrintF(out, "\r\n");
    }
}
