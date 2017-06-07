#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "base.h"
#include "task.h"
#include "t_client.h"
#include "cmd.h"
#include "io.h"

struct tagCmd {
    char scmd[128];
    char sarg[5][128];
    int  uarg[5];
    int  ret;
    void * pfunc;
    struct tagCmd * pnext;
};

typedef int (*cmdfunchook)(char *,struct tagCmd *);

struct tagCmd * cmdHead = NULL;
struct tagCmd * cmdCur  = NULL;

char *cmdargv[100] = {0};
int   cmdargnum = 0;

char lastcmd[1024] = {0};

int cmd_enter(char * nbuf , struct tagCmd * pcmd )
{
    int length;
    int i;
    int j = sizeof(pcmd->sarg) / sizeof(pcmd->sarg[0]);

    length = strlen(pcmd->sarg[0]);

    pcmd->sarg[0][length] = '\r';
    pcmd->sarg[0][length+1] = '\n';

    Telnet_Send(pcmd->sarg[0], length + 2);

    for( i = 1 ; i < j ; i++ )
    {
        length = strlen(pcmd->sarg[i]);
        if(length)
        {
            Task_Delay(1000);
            pcmd->sarg[i][length] = '\r';
            pcmd->sarg[i][length+1] = '\n';
            Telnet_Send(pcmd->sarg[i], length + 2);
        }
    }

    Task_Delay(100);

    return 0;
}

int cmd_sleep(char * nbuf , struct tagCmd * pcmd )
{
    Task_Delay(pcmd->uarg[0]);
    return 0;
}

int cmd_stop(char * nbuf , struct tagCmd * pcmd )
{
    if( nbuf )
    {
        if( strstr(nbuf,pcmd->sarg[0]) )
        {
            cmdCur = NULL;
            pcmd->ret = 1;
        }
    }
    else
    {
        if( !strstr(lastcmd,pcmd->sarg[0]) )
        {
            cmdCur = pcmd;
            Task_Delay(pcmd->uarg[0]);
        }
        else
        {
            pcmd->ret = 1;
        }

        if( pcmd->ret )
        {
            while(Task_Run())
            {
                Task_Delay(1000);
            }
        }
    }

    return 0;
}


int cmd_wait(char * nbuf , struct tagCmd * pcmd )
{
    if( nbuf )
    {
        if( strstr(nbuf,pcmd->sarg[0]) )
        {
            cmdCur = NULL;
            pcmd->ret = 1;
        }
    }
    else
    {
        if( !strstr(lastcmd,pcmd->sarg[0]) )
        {
            cmdCur = pcmd;
            while(!pcmd->ret)
            {
                Task_Delay(10);
            }
        }
        else
        {
            pcmd->ret = 1;
        }
    }

    return 0;
}

int cmd_waitt(char * nbuf , struct tagCmd * pcmd )
{
    if( nbuf )
    {
        if( strstr(nbuf,pcmd->sarg[0]) )
        {
            cmdCur = NULL;
            pcmd->ret = 1;
        }
    }
    else
    {
        if( !strstr(lastcmd,pcmd->sarg[0]) )
        {
            cmdCur = pcmd;
            while(!pcmd->ret)
            {
                Telnet_Send("t\r\n", 3);
                Task_Delay(1000);
            }
        }
        else
        {
            pcmd->ret = 1;
        }
    }

    return 0;
}

int cmd_waitenter(char * nbuf , struct tagCmd * pcmd )
{
    if( nbuf )
    {
        if( strstr(nbuf,pcmd->sarg[0]) )
        {
            cmdCur = NULL;
            pcmd->ret = 1;
        }
    }
    else
    {
        if( !strstr(lastcmd,pcmd->sarg[0]) )
        {
            cmdCur = pcmd;
            while(!pcmd->ret)
            {
                Telnet_Send("\r\n", 2);
                Task_Delay(1000);
            }
        }
        else
        {
            pcmd->ret = 1;
        }
    }

    return 0;
}

int cmd_ifexit(char * nbuf , struct tagCmd * pcmd )
{
    if( nbuf )
    {
        if( strstr(nbuf,pcmd->sarg[0]) )
        {
            cmdCur = NULL;
            pcmd->ret = 1;
        }
    }
    else
    {
        if( !strstr(lastcmd,pcmd->sarg[0]) )
        {
            cmdCur = pcmd;
            while(!pcmd->ret)
            {
                Task_Delay(100);
            }
        }
        else
        {
            pcmd->ret = 1;
        }
        Task_Delete();
        Task_Delay(1000);
    }

    return 0;
}

int cmd_exitsys(char * nbuf , struct tagCmd * pcmd )
{
    Task_Delete();
    return 0;
}

int cmd_print(char * nbuf , struct tagCmd * pcmd )
{
    IO_WinPrint(TRUE);
    return 0;
}

int cmd_noprint(char * nbuf , struct tagCmd * pcmd )
{
    IO_WinPrint(FALSE);
    return 0;
}

int cmd_ifelse(char * nbuf , struct tagCmd * pcmd )
{
    if( nbuf )
    {
        if( strstr(nbuf,pcmd->sarg[0]) )
        {
            cmdCur = NULL;
            pcmd->ret = 1;
        }
    }
    else
    {
        int count = 0;
        int length;

        if( !strstr(lastcmd,pcmd->sarg[0]) )
        {
            cmdCur = pcmd;
            while(!pcmd->ret && count < pcmd->uarg[0] / 10  )
            {
                count++;
                Task_Delay(10);
            }
        }
        else
        {
            pcmd->ret = 1;
        }

        if( pcmd->ret )
        {
            length = strlen(pcmd->sarg[1]);
            if(length)
            {
                pcmd->sarg[1][length] = '\r';
                pcmd->sarg[1][length+1] = '\n';
                Telnet_Send(pcmd->sarg[1], length + 2);
            }
        }
        else
        {
            length = strlen(pcmd->sarg[2]);
            if(length)
            {
                pcmd->sarg[2][length] = '\r';
                pcmd->sarg[2][length+1] = '\n';
                Telnet_Send(pcmd->sarg[2], length + 2);
            }
            cmdCur = NULL;
        }
    }

    return 0;
}

int cmd_waittimeout(char * nbuf , struct tagCmd * pcmd )
{
    if( nbuf )
    {
        if( strstr(nbuf,pcmd->sarg[0]) )
        {
            cmdCur = NULL;
            pcmd->ret = 1;
        }
    }
    else
    {
        int count = 0;

        if( !strstr(lastcmd,pcmd->sarg[0]) )
        {
            cmdCur = pcmd;
            while(!pcmd->ret && count < pcmd->uarg[0] / 10  )
            {
                count++;
                Task_Delay(10);
            }
            cmdCur = NULL;
        }
        else
        {
            pcmd->ret = 1;
        }
    }

    return 0;
}

int cmd_while(char * nbuf , struct tagCmd * pcmd )
{
    if( nbuf )
    {
        if( strstr(nbuf,pcmd->sarg[0]) )
        {
            cmdCur = NULL;
            pcmd->ret = 1;
        }
    }
    else
    {
        int length;

        length = strlen(pcmd->sarg[1]);
        if(length)
        {
            pcmd->sarg[1][length] = '\r';
            pcmd->sarg[1][length+1] = '\n';

            if( !strstr(lastcmd,pcmd->sarg[0]) )
            {
                cmdCur = pcmd;
                while(!pcmd->ret )
                {
                    Telnet_Send(pcmd->sarg[1], length + 2);
                    Task_Delay(1000);
                }
            }
        }

        pcmd->ret = 1;
    }

    return 0;
}

int cmd_for(char * nbuf , struct tagCmd * pcmd )
{
    int length;
    int i;

    length = strlen(pcmd->sarg[0]);
    if(length)
    {
        pcmd->sarg[0][length] = '\r';
        pcmd->sarg[0][length+1] = '\n';

        for( i = 0 ; i < pcmd->uarg[0] ; i++ )
        {
            Telnet_Send(pcmd->sarg[0], length + 2);
            Task_Delay(1000);
        }
    }

    return 0;
}


int CMD_SHOW_RUN()
{
    struct tagCmd * pcmd = cmdHead;

    while(pcmd)
    {
        int i;

        printf("---------------------------------------\r\n");
        printf("cmd : %s \r\n",pcmd->scmd);

        for(i = 0 ; i < 5 ; i++ )
        {
            if('\0' == pcmd->sarg[i][0])
            {
                printf("scmd[%u] : null \r\n",i);
            }
            else
            {
                printf("scmd[%u] : %s \r\n",i,pcmd->sarg[i]);
            }
        }

        for(i = 0 ; i < 5 ; i++ )
        {
            printf("sarg[%u] : %u \r\n",i,pcmd->uarg[i]);
        }

        printf("func : %p \r\n",pcmd->pfunc);
        printf("ret  : %u \r\n",pcmd->ret);

        pcmd = pcmd->pnext;
    }

    return 0;
}


void CMD_RUN()
{
    struct tagCmd * pcmd = cmdHead;
    cmdfunchook pfun;

    if(!cmdHead)
    {
        return;
    }

#ifdef DEBUG
    CMD_SHOW_RUN();
#endif

    while(pcmd)
    {
        Task_Delay(1000);
        pfun = (cmdfunchook)pcmd->pfunc;
        if( pfun )
        {
            (void)pfun(NULL,pcmd);
        }
        pcmd = pcmd->pnext;
    }

    return;
}

void CMD_Check(char * nbuf)
{
    cmdfunchook pfun;
    struct tagCmd * pcmd = cmdCur;

    if(!cmdHead)
    {
        return;
    }

    if( pcmd && pcmd->pfunc)
    {
        char allcmd[2048] = {0};
        pfun = (cmdfunchook)pcmd->pfunc;
        snprintf(allcmd, 2048, "%s%s",lastcmd,nbuf);
        (void)pfun(allcmd,pcmd);
    }

    strncpy(lastcmd,nbuf,1024);
}

char * strnchr(char * str,char c)
{
    for( ; *str == c ; str++ )
    {
        if( '\0' == str )
        {
            return NULL;
        }
    }

    return str;
}


char * TransMean(char * cmd)
{
    int i,j;
    char num[3];

    for( i = 0 , j = 0 ; '\0' != cmd[i] ; i++ )
    {
        if( '\\' == cmd[i] )
        {
            int n;

            if('\\' == cmd[++i] )
            {
                cmd[j++] = '\\';
                continue;
            }

            for( n = 0 ; n < 3 && isdigit(cmd[i]) ; n++ , i++ )
            {
                num[n] = cmd[i];
            }

            if( n )
            {
                cmd[j++] = atoi(num);
            }

            if( '\0' == cmd[i] )
            {
                break;
            }

            i--;
        }
        else
        {
            cmd[j++] = cmd[i];
        }
    }

    cmd[j++] = '\0';
    return cmd;
}


int Add_Cmd(char * cmd)
{
    int    length;
    char * pcmdname;
    char * pchar;
    struct tagCmd * pnew;
    struct tagCmd temp;
    static struct tagCmd * pnext = NULL;

    (void)TransMean(cmd);

    memset(&temp,0,sizeof(struct tagCmd));

    /* sleep 1001 */
    pcmdname = "sleep ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length - 1);
        temp.uarg[0] = atoi(cmd + length);
        temp.pfunc = cmd_sleep;
        goto ADD;
    }

    /* stop 10000 string */
    pcmdname = "stop ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length -1);

        temp.uarg[0] = atoi(cmd + length);

        if( temp.uarg[0] )
        {
            pchar = strchr(cmd + length,' ');
            if( pchar )
            {
                pchar = strnchr(pchar ,' ');
                if(pchar)
                {
                    snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
                }
            }
        }
        else
        {
            temp.uarg[0] = 1000;

            pchar = strnchr(cmd + length,' ');
            if(pchar)
            {
                snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
            }
        }

        temp.pfunc = cmd_stop;
        goto ADD;
    }

    /* wait string */
    pcmdname = "wait ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length -1);
        pchar = strnchr(cmd + length,' ');
        if(pchar)
        {
            snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
        }
        temp.pfunc = cmd_wait;
        goto ADD;
    }

    /* waitt string */
    pcmdname = "waitt ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length -1);
        pchar = strnchr(cmd + length,' ');
        if(pchar)
        {
            snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
        }
        temp.pfunc = cmd_waitt;
        goto ADD;
    }

    /* waitt string */
    pcmdname = "waitenter ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length -1);
        pchar = strnchr(cmd + length,' ');
        if(pchar)
        {
            snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
        }
        temp.pfunc = cmd_waitenter;
        goto ADD;
    }

    /* ifexit string */
    pcmdname = "ifexit ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length -1);
        pchar = strnchr(cmd + length,' ');
        if(pchar)
        {
            snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
        }
        temp.pfunc = cmd_ifexit;
        goto ADD;
    }

    /* exitsys */
    pcmdname = "exitsys";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length);
        temp.pfunc = cmd_exitsys;
        goto ADD;
    }

    /* print */
    pcmdname = "print";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length);
        temp.pfunc = cmd_print;
        goto ADD;
    }

    /* noprint */
    pcmdname = "noprint";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length);
        temp.pfunc = cmd_noprint;
        goto ADD;
    }

    /* ifelse 1000 wait_str yes_str no_str */
    pcmdname = "ifelse ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        char * pchar2;
        char * pchar3;
        char * pchar4;
        char * pchar5;

        temp.pfunc = cmd_ifelse;
        strncpy(temp.scmd,pcmdname,length - 1);

        temp.uarg[0] = atoi(cmd + length);

        if( 0 == temp.uarg[0] )
        {
            goto ADD;
        }

        pchar = strchr(cmd + length ,' ');
        if( 0 == pchar )
        {
            goto ADD;
        }

        pchar = strnchr(pchar ,' ');
        if( 0 == pchar )
        {
            goto ADD;
        }

        pchar2 = strchr(pchar,' ');
        if( 0 == pchar2 )
        {
            goto ADD;
        }

        pchar3 = strnchr(pchar2,' ');
        if( 0 == pchar3 )
        {
            goto ADD;
        }

        pchar4 = strchr(pchar3,' ');
        if( pchar4 )
        {
            pchar5 = strnchr(pchar4,' ');
            if( pchar5 )
            {
                *pchar2 = '\0';
                *pchar4 = '\0';
                snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
                snprintf(temp.sarg[1],sizeof(temp.sarg[1]),"%s",pchar3);
                snprintf(temp.sarg[2],sizeof(temp.sarg[2]),"%s",pchar5);
            }
        }
        else
        {
            *pchar2 = '\0';
            snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
            snprintf(temp.sarg[1],sizeof(temp.sarg[1]),"%s",pchar3);
        }

        goto ADD;
    }

    /* waittimeout 1000 wait_str */
    pcmdname = "waittimeout ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length -1);

        temp.uarg[0] = atoi(cmd + length);

        if( temp.uarg[0] )
        {
            pchar = strchr(cmd + length,' ');
            if( pchar )
            {
                pchar = strnchr(pchar ,' ');
                if(pchar)
                {
                    snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
                }
            }
        }

        temp.pfunc = cmd_waittimeout;
        goto ADD;
    }

    /* while wait_str user_cmd */
    pcmdname = "while ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        char * pchar2;
        char * pchar3;

        strncpy(temp.scmd,pcmdname,length - 1);

        pchar = strnchr(cmd + length ,' ');
        if(pchar)
        {
            pchar2 = strchr(pchar,' ');
            if( pchar2 )
            {
                pchar3 = strnchr(pchar2,' ');
                if( pchar3 )
                {
                    *pchar2 = '\0';
                    snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
                    snprintf(temp.sarg[1],sizeof(temp.sarg[1]),"%s",pchar3);
                }
            }
        }

        temp.pfunc = cmd_while;
        goto ADD;
    }

    /* for 10 user_cmd */
    pcmdname = "for ";
    length   = strlen(pcmdname);
    if(!strncmp(cmd,pcmdname,length))
    {
        strncpy(temp.scmd,pcmdname,length - 1);
        temp.uarg[0] = atoi(cmd + length);

        if( temp.uarg[0] )
        {
            pchar = strchr(cmd + length,' ');
            if( pchar )
            {
                pchar = strnchr(pchar ,' ');
                if(pchar)
                {
                    snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
                }
            }
        }

        temp.pfunc = cmd_for;

        goto ADD;
    }

    /* user_cmd */
    if(cmd[0] != '\0')
    {
        pchar = strnchr(cmd ,' ');
        if(pchar)
        {
            snprintf(temp.sarg[0],sizeof(temp.sarg[0]),"%s",pchar);
        }
    }

    strcpy(temp.scmd,"enter");
    temp.pfunc = cmd_enter;

ADD:

    pnew = malloc(sizeof(struct tagCmd));
    if( NULL == pnew )
    {
        LOG("malloc(%u) failed!",sizeof(struct tagCmd));
        return -1;
    }

    memcpy(pnew,&temp,sizeof(struct tagCmd));
    pnew->pnext  = NULL;
    if( pnext )
    {
        pnext->pnext = pnew;
    }
    else
    {
        cmdHead = pnew;
    }
    pnext = pnew ;
    return 0;
}

void cmd_argx(char * line,int n)
{
    char * p;
    int  num;
    char getline[2048];

    strncpy(getline,line,2048);

    while(NULL != (p = strstr(getline,"%")))
    {
        char stemp[2048];

        *p = '\0';
        p++;

        num = atoi(p);

        while(isdigit(*p))
        {
            p++;
        }

        if(num < cmdargnum )
        {
            snprintf(stemp,2048,"%s%s%s",getline,cmdargv[num],p);
        }
        else
        {
            snprintf(stemp,2048,"%s%s",getline,p);
        }

        strncpy(getline,stemp,2048);
    }

    strncpy(line,getline,n);
}

int Read_Ini(char * ini)
{
    int err;
    char getline[1024];
    FILE * pf = fopen(ini,"r");
    if( NULL == pf )
    {
        LOG("open ini %s failed!",ini);
        return -1;
    }

    while(fgets(getline, 1024,pf))
    {
        char * ptemp;

        if('#' == getline[0])
        {
            continue;
        }
        else if(strstr(getline,"[command]"))
        {
            continue;
        }

        ptemp = strchr(getline,'\n');
        if(ptemp)
        {
            *ptemp = '\0';
        }

        ptemp = strchr(getline,'\r');
        if(ptemp)
        {
            *ptemp = '\0';
        }

        cmd_argx(getline,1024);

        if(strstr(getline,"[host]"))
        {
            strcpy(server_ip,&getline[6]);
        }
        else if(strstr(getline,"[port]"))
        {
            server_port = atoi(&getline[6]);
        }
        else if(strstr(getline,"[exit]"))
        {
            time_out = atoi(&getline[6]);
        }
        else
        {
            err = Add_Cmd(getline);
            if(0 != err)
            {
                LOG("get error cmd! (%s)",getline);
                fclose(pf);
                return -1;
            }
        }
    }

    fclose(pf);

    return 0;
}

/* 格式化参数 */
int CMD_Argx(int argc,char ** argv)
{
    int i;
    for( i = 0 ; i < argc && cmdargnum < 100 ; i++ )
    {
        cmdargv[i] = argv[i];
        cmdargnum++;
    }

    return 0;
}

int CMD_Init(char * ini)
{
    int err;

    err = Read_Ini(ini);
    if(err)
    {
        return err;
    }

    return 0;
}

int CMD_Fini()
{
    return 0;
}

