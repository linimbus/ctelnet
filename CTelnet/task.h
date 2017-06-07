#ifndef __TASK_H__
#define __TASK_H__

typedef void (*ThreadProc)();

int Task_Create(ThreadProc pfun);

void Task_Delete();

int Task_Run();

int Task_Fini();

void Task_Delay(int sec);

#endif

