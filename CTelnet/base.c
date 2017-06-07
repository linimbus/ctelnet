#include "base.h"
#include "io.h"

char * version = "0.2.3";
char server_ip[32];
int  server_port    = 23;
char filename[1024];
int  time_out = 0;
int  deamon_run = 0;

void DEBUG_CHECK()
{
#ifdef DEBUG

#if (OS == WIN32)
    printf("Press ENTER to continue...");
    IO_Get();
#endif

#endif
}


void Debug(const char *fmt, ...)
{
    int nBuf;
    char szBuffer[1024];
    va_list args;

    va_start(args, fmt);
    nBuf = vsnprintf(szBuffer,1024,fmt,args);
    va_end(args);

    write(STD_ERR,szBuffer,nBuf);
}

