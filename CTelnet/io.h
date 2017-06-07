#ifndef __IO_h_
#define __IO_h_

int IO_Init();

void IO_Format(const char *format, ...);

void IO_Wite(void* szBuffer, int nBuf);

int IO_Fini();

void IO_Wait();

char IO_Get();

void IO_WinPrint(int flag);

#endif

