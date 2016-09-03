#include <fcgiapp.h>
int main(void)
{
    int result = 0;
    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);
    while(FCGX_Accept_r(&request) == 0) {
    }
    return result;
}
