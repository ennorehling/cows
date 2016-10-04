#pragma once

#define MAXHEADERS 8
#define MAXCOOKIES 8

typedef struct cw_response {
    char *headers[MAXHEADERS*2];
    char *cookies[MAXCOOKIES*2];
    int http_code;
    char *status;
} cw_response;

struct FCGX_Stream;

void cw_res_init(struct cw_response *cr);
void cw_res_free(struct cw_response *cr);

void cw_res_set_status(struct cw_response *cr, int http_code, const char *message);
int cw_res_set_header(struct cw_response *cr, const char *name, const char *value);
int cw_res_set_cookie(struct cw_response *cr, const char *name, const char *value);

void cw_res_print_headers(struct cw_response *cr, struct FCGX_Stream *out);
