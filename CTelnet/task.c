
#include "base.h"
#include "task.h"

#if (OS == WIN32)
#include <windows.h>
#elif (OS == LINUX)
#include <pthread.h>
#include <sys/select.h>
#endif


#define MAX_TASK_NUM 100

#define TASK_S_DEL 0
#define TASK_S_RUN 1

#if (OS == WIN32)
HANDLE hThread[MAX_TASK_NUM] = {0};
#elif (OS == LINUX)
pthread_t hThread[MAX_TASK_NUM] = {0};
#endif

int g_tasknum    = 0;
int g_taskfinish = TASK_S_RUN;

int Task_Create(ThreadProc pfun)
{
#if (OS == WIN32)
    DWORD threadID;
#elif (OS == LINUX)
    int ulRet;
#endif

    if( g_tasknum >= MAX_TASK_NUM )
    {
        LOG("create task too mouch");
        return -1;
    }

#if (OS == WIN32)
    hThread[g_tasknum] = CreateThread(NULL,
                                      0,
                                      (LPTHREAD_START_ROUTINE)pfun,
                                      NULL,
                                      0,
                                      &threadID);
    if(NULL == hThread[g_tasknum] )
    {
        LOG("create task failed");
        return -1;
    }
#elif (OS == LINUX)
    ulRet = pthread_create( &hThread[g_tasknum],
      						NULL ,
      						(void*(*)(void*))pfun,
      						NULL );
	if(0 != ulRet )
    {
        LOG("create task failed");
        return -1;
    }
#endif

    g_tasknum++;

    return 0;
}

void Task_Delete()
{
    g_taskfinish = TASK_S_DEL;
}

int Task_Run()
{
    return g_taskfinish;
}

int Task_Fini()
{
#if (OS == WIN32)

    int err;
    err = WaitForMultipleObjects(g_tasknum, hThread, TRUE, INFINITE);
    if(err)
    {
        LOG("wait for task finish failed!");
        return -1;
    }
#elif (OS == LINUX)

    void * pRetVal = NULL;
    int i;

    for( i = 0 ; i < g_tasknum; i++ )
    {
        (void)pthread_join(hThread[i], &pRetVal);
    }

#endif

    return 0;
}

void Task_Delay(int ms)
{
#if (OS == WIN32)
    Sleep(ms);
#elif (OS == LINUX)
    struct timeval tv;
    tv.tv_sec  = (ms / 1000);
    tv.tv_usec = (ms % 1000) * 1000;
    select(0, NULL, NULL, NULL, &tv);
#endif
}

