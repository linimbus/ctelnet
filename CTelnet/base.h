#ifndef __common_h_
#define __common_h_

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE 	0
#endif

#ifndef STD_IN
#define STD_IN	0
#endif

#ifndef STD_OUT
#define STD_OUT	1
#endif

#ifndef STD_ERR
#define STD_ERR	2
#endif

/* OS */
#ifndef LINUX
#define LINUX 2
#endif

#ifndef WIN32
#define WIN32 1
#endif

#ifndef OS
#define OS WIN32
#endif

#if (OS == WIN32)
#define snprintf _snprintf
#endif

#if (OS == WIN32)
#define vsnprintf _vsnprintf
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>


#if (OS == WIN32)
#include <conio.h>
#undef getch
extern int getch();
#elif(OS == LINUX)
#include <curses.h>
#include <unistd.h>
#include <time.h>
#endif

extern char * version;
extern char server_ip[];
extern int  server_port;
extern int time_out;
extern char filename[];
extern int deamon_run;

void Debug(const char *fmt, ...);

void DEBUG_CHECK();

#define LOG(fmt,...) \
    Debug("[FUNC(%s)-LINE(%u)]"fmt,__func__,__LINE__,##__VA_ARGS__);

#endif

