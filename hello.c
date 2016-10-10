#include "response.h"

#include <fcgiapp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int http_response(FCGX_Stream *out, int http_code, const char *message,
    const char *body, size_t length)
{
    cw_response resp;

    cw_res_init(&resp);
    cw_res_set_status(&resp, http_code, message);
    cw_res_set_cookie(&resp, "foo", "bar");
    cw_res_set_cookie(&resp, "barf", "food");
    cw_res_print_headers(&resp, out);
    cw_res_set_header(&resp, "Content-Type", "test/plain");
    FCGX_FPrintF(out,
                 "Content-Length: %u\r\n"
                 "\r\n", (unsigned int)length);
    if (body) {
        FCGX_PutStr(body, (int)length, out);
    }
    cw_res_free(&resp);
    return http_code;
}


static int http_success(FCGX_Stream *out, const char *body) {
    size_t size = 0;
    const char * data = "";
    if (body) {
        data = body;
        size = strlen(body);
    }
    return http_response(out, 200, "OK", data, size);
}

static int hello(FCGX_Request *req) {
    return http_success(req->out, "Hello World");
}

static int ipv4_addr(const char *str) {
    const char *p = str;
    int a = 0;
    while (p && *p) {
        a = (a << 8) + atoi(p);
        p = strchr(p, '.');
        if (p) ++p;
    }
    return a;
}

static int session(FCGX_Request *req) {
    char buf[64];
    const char *cookie = FCGX_GetParam("HTTP_COOKIE", req->envp);
    snprintf(buf, sizeof(buf), "cookie: %s\n", cookie);
    return http_success(req->out, buf);
}

static int counter(FCGX_Request *req) {
    char buf[64];
    static unsigned int c = 0, u = 0;
    const char * addr = FCGX_GetParam("REMOTE_ADDR", req->envp);
    static int last_visitor = 0;
    int visitor;
    
    visitor = ipv4_addr(addr);
    if (last_visitor!=visitor) {
        ++u;
        last_visitor = visitor;
    }
    ++c;
    snprintf(buf, sizeof(buf), "visitors: %u\nunique: %u", c, u);
    return http_success(req->out, buf);
}

static int process(FCGX_Request *req) {
    const char *method = FCGX_GetParam("REQUEST_METHOD", req->envp);
    const char *script = FCGX_GetParam("REQUEST_URI", req->envp);
    printf("%s request for %s\n", method, script);
    if (strcmp(script, "/session")==0) {
        return session(req);
    }
    if (strcmp(script, "/hello")==0) {
        return hello(req);
    }
    if (strcmp(script, "/counter")==0) {
        return counter(req);
    }
    return http_response(req->out, 404, "Not Found", NULL, 0);
}

int main(void)
{
    int result = 0;
    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);
    do {
        result = FCGX_Accept_r(&request);
        if (result==0) {
           result = process(&request);
        } else {
            fprintf(stderr, "FCGX_Accept returned %d\n", result);
        }
    } while (result>=0);
    return result;
}
