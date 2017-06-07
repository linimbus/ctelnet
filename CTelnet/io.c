#include "base.h"
#include "io.h"
#include "cmd.h"

#include <stdarg.h>
#include <stdio.h>

#include <sys/time.h>
#include <sys/types.h>

#if(OS == LINUX)
#include <time.h>
#endif

extern char *cmdargv[100];
extern int   cmdargnum;

FILE * fileopen;

int m_win_print_switch = TRUE;

void IO_WinPrint(int flag)
{
    m_win_print_switch = flag;
}

void IO_Info()
{
    int i;
    time_t timenow;
    time(&timenow);

    IO_Format("===============================================\r\n");
    IO_Format(" IP      : %s \r\n",server_ip);
    IO_Format(" PORT    : %u \r\n",server_port);
    IO_Format(" TimeOut : %u(s)\r\n",time_out);
    IO_Format(" TimeNow : %s",asctime(localtime(&timenow)));
    IO_Format(" Version : %s \r\n",version);

    if( cmdargnum )
    {
        IO_Format(" argnum  : %u \r\n",cmdargnum);

        for( i = 0 ; i < cmdargnum ; i++ )
        {
            IO_Format(" arg%2u   : %s \r\n",i,cmdargv[i]);
        }
    }

    IO_Format("===============================================\r\n");
}

int IO_Init()
{
    fileopen = fopen(filename, "w");
    if(NULL == fileopen)
    {
        LOG("open %s failed!",filename);
        return -1;
    }

    IO_Info();

#if (OS == LINUX)

    if( !deamon_run )
    {
        initscr();
        noecho();
    }

#endif

    return 0;
}

void IO_Format(const char *format, ...)
{
    int  nBuf;
    char szBuffer[1024];
    va_list args;

    va_start(args, format);
    nBuf = vsnprintf(szBuffer,1024,format,args);
    va_end(args);

    fwrite(szBuffer, nBuf, 1, fileopen);

    if( m_win_print_switch && !deamon_run )
    {
        write(STD_OUT,szBuffer,nBuf);
    }
    CMD_Check(szBuffer);
}

void IO_Wite(void* szBuffer, int nBuf)
{
    fwrite(szBuffer, nBuf, 1, fileopen);
    if( m_win_print_switch && !deamon_run )
    {
        write(STD_OUT,szBuffer,nBuf);
    }
    CMD_Check(szBuffer);
}

int IO_Fini()
{
    fclose(fileopen);

#if (OS == LINUX)

    if( !deamon_run )
    {
        nonl();
        endwin();
    }

#endif

    return 0;
}

char IO_Get()
{
    return getch();
}

