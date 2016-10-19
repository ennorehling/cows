#include "response.h"
#include <quicklist/quicklist.h>
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
    ql_foreach(res->headers, free);
    ql_free(res->headers);
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

typedef struct cw_header {
    char *name;
    char *value;
} cw_header;

int cw_res_set_header(cw_response *res, const char *name, const char *value)
{
    ql_iter iter;
    cw_header *hdr;
// TODO: use the set functions to make this a binary search
    for(iter = qli_init(&res->headers);qli_more(iter);) {
        hdr = (cw_header *)qli_next(&iter);
        if (strcmp(hdr->name, name)==0) {
            free(hdr->value);
            hdr->value = _strdup(value);
            return 0;
        }
    }
    hdr = malloc(sizeof(cw_header));
    hdr->name = _strdup(name);
    hdr->value = _strdup(value);
    ql_push(&res->headers, hdr);
    return 0;
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
    ql_iter iter;
    FCGX_FPrintF(out,
                 "Status: %d %s\r\n",
                 res->http_code ? res->http_code : 200,
                 res->status ? res->status : "Okay");
    for (iter=qli_init(&res->headers);qli_more(iter);) {
        cw_header * hdr=(cw_header *)qli_next(&iter);
        FCGX_FPrintF(out, "%s: %s\r\n", hdr->name, hdr->value);
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
