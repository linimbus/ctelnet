
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#include "base.h"
#include "task.h"
#include "t_client.h"
#include "io.h"
#include "cmd.h"

#define USER_BUF_SIZE	256

extern tel_ctrl m_Tel_info;
const char calogout[] = "telnet logout\r\n";
int get_msg_count = 0;


void Msg_Parse(char * msg_to_user,int length)
{
    int i,j;
    char * pc;

    for(i = 0,j = 0;j < length; j++ )
    {
        if( '\e' == msg_to_user[j] )
        {
            pc = strstr(msg_to_user + j , "m");
            if(pc)
            {
                j = pc - msg_to_user;
            }
            continue;
        }
        else if('\0' == msg_to_user[j])
        {
            continue;
        }
        else if('\7' == msg_to_user[j])
        {
            msg_to_user[j] = ' ';
        }

        msg_to_user[i++] = msg_to_user[j];
    }

    msg_to_user[i] = '\0';
}

void REV_TASK()
{
    int length;
    int err;
    char msg_to_user[1025] = {0};
    while(Task_Run())
    {
        length = 1024;
        err = Telnet_Get((unsigned char *)msg_to_user,&length);
		if(-1 == err)
		{
            Task_Delete();
            return;
		}
		else if( -2 == err)
		{
            IO_Format(calogout);
            Task_Delete();
            return;
		}
		else
		{
		    Msg_Parse(msg_to_user,length);
            IO_Format("%s",msg_to_user);
		}

		get_msg_count++;
    }
}

/* time_out 秒之后没有收到消息就自动断开连接 */
void CLOCK_CHECK()
{
    int longtimeflag = FALSE;
    int temp  = 0;
    int count = 0;

    while(Task_Run())
    {
        Task_Delay(1000);

        /* time_out 等于零表示不进行检测 */
        if( 0 == time_out  )
        {
            continue;
        }

        if( count == get_msg_count )
        {
            if( longtimeflag )
            {
                temp++;
                if( temp >= time_out)
                {
                    IO_Format("\r\ntelnet time out %u(s)",temp);
                    Task_Delete();
                    return;
                }
            }
            else
            {
                temp = 0;
                longtimeflag = TRUE;
            }
        }
        else
        {
            count = get_msg_count;
            longtimeflag = FALSE;
        }
    }
}


void USER_TASK()
{
    char onechar;

    if( deamon_run )
    {
        return;
    }

    while(Task_Run())
    {
        if(Telnet_Run())
        {
            onechar = IO_Get();
            Telnet_Send(&onechar,1);
            //fflush(stdout);
        }
    }
}

int main(int argc,char **argv)
{
    int err;
    time_t timenow;
    time(&timenow);

    if(argc == 1)
    {
        LOG("Usage 1:%s server_ip [server_port]\n",argv[0]);
        LOG("Usage 2:%s run.ini\n",argv[0]);
        DEBUG_CHECK();
		return -1;
    }
    else if(strstr(argv[1],".ini"))
    {
        /* 说明带有参数方式的脚本执行 */
        if( argc > 2 )
        {
            CMD_Argx(argc - 2 , &argv[2] );
        }

        err = CMD_Init(argv[1]);
        if(err)
        {
            DEBUG_CHECK();
            return -1;
        }

        sprintf(filename,"%s.result.txt",argv[1]);

        if( argc == 3 && strstr(argv[2],"-D") )
        {
            deamon_run = 1;
        }
    }
    else if(argc == 2)
    {
		strcpy(server_ip,argv[1]);
        sprintf(filename,"telnet_report_%lu.txt",timenow);
        server_port = 23;
        time_out    = 0;
	}
	else if(argc == 3)
	{
		strcpy(server_ip,argv[1]);
		server_port = atoi(argv[2]);
        sprintf(filename,"telnet_report_%lu.txt",timenow);
        time_out    = 0;
	}
	else
	{
		LOG("Usage:%s server_ip [server_port]\n",argv[0]);
        DEBUG_CHECK();
		return -1;
	}

	err = IO_Init();
    if(err)
    {
        DEBUG_CHECK();
        return -1;
    }

    err = Telnet_Init();
	if(err)
    {
        IO_Fini();
        DEBUG_CHECK();
        return -1;
    }

    err  = Task_Create(REV_TASK);
    if(err)
    {
        DEBUG_CHECK();
        return -1;
    }

    err  = Task_Create(USER_TASK);
    err |= Task_Create(CMD_RUN);
	if(err)
    {
        Telnet_Fini();
        IO_Fini();
        DEBUG_CHECK();
        return -1;
    }

    /* 进行超时检测 */
    CLOCK_CHECK();

    err  = Telnet_Fini();
    err |= IO_Fini();
	if(err)
    {
        return -1;
    }

    Task_Delay(1000);
	return 0;
}


