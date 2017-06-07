#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#if (OS == LINUX)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "base.h"
#include "t_client.h"
#include "task.h"
#include "io.h"

tel_ctrl m_Tel_info;

static void create_sub_op(unsigned char op,unsigned char num,char *in_str,unsigned char *out_str,int *option_len)
{
	int string_size=0;

	*out_str++ = op;
	*out_str++ = num;
	while((*(out_str+string_size)=*(in_str+string_size)) != '\0'){
		string_size++;
	};
	out_str += string_size;
	*out_str++ = CMD_IAC;
	*out_str++ = CMD_SE;

	*option_len = 4+string_size;
}

static int get_cmd_option(unsigned char *buf,unsigned char *cmd,unsigned char *option,int *used_len,int spare_len)
{
	int i=0;

	if((*buf != CMD_IAC) || ((*buf == CMD_IAC) &&(*(buf+1) == CMD_IAC)))
	{
        IO_Wite(buf, spare_len);
		return -1;
	}
	i++;

	*cmd = *(buf+i++);
	if(*cmd == CMD_SB)
	{
		while(i<spare_len)
		{
			*option++ = *(buf+i);
			if((*(buf+i-1) == CMD_IAC) && (*(buf+i) == CMD_SE))
			{
				i++;
				break;
			}
			i++;
		}
	}
	else
	{
		*option++ = *(buf+i++);
	}

	*used_len = i;

	return 0;
}

static void create_ret_pack(unsigned char *buf,unsigned char cmd,unsigned char *option,int *used_len)
{
	unsigned char cmd_send=0;
	int option_len=0,i;
	unsigned char op[32];

	*buf++ = CMD_IAC;
	switch (cmd){
		case CMD_DO:
			switch (*option){
				case OP_ECHO:
				case OP_SUP_GA:
				case OP_BIN_TRANS:
				case OP_TERM_TYPE:
					cmd_send = CMD_WILL;
					op[0] = *option;
					break;
				default:
					op[0] = *option;
					cmd_send = CMD_WONT;
					break;
			}
			option_len = 1;
			break;
		case CMD_WILL:
			switch (*option){
				//case OP_ECHO:   //关闭好一点，免得看起来都隔了一行
				case OP_SUP_GA:
				case OP_BIN_TRANS:
				case OP_TERM_TYPE:
					cmd_send = CMD_DO;
					op[0] = *option;
					break;
				default:
					cmd_send = CMD_DONT;
					op[0] = *option;
					break;
			}
			option_len = 1;
			break;
		case CMD_DONT:
			cmd_send = CMD_WONT;
			op[0] = *option;
			option_len = 1;
			break;
		case CMD_WONT:
			cmd_send = CMD_DONT;
			op[0] = *option;
			option_len = 1;
			break;
		case CMD_SB:
			switch (*option){
				case OP_TERM_TYPE:
					cmd_send = CMD_SB;
					create_sub_op(OP_TERM_TYPE,0,MY_TERM_TYPE,op,&option_len);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	*buf++ = cmd_send;
	for(i=0;i<option_len;i++){
		*(buf+i) = op[i];
	}
	*used_len = 1+1+option_len;
}

/* 根据自己的情况与服务器协商 */
static void negotiate(unsigned char *buf,unsigned char *msg_to_svr,int buf_len,int *p_msglen)
{
	int tmp_len=0,used_len=0,spare_len;
	int used_len1=0,total_len=0;
	unsigned char cmd=0;
	unsigned char option[32];

	spare_len = buf_len;
	while(spare_len>0)
	{
		if(get_cmd_option(buf+used_len,&cmd,option,&tmp_len,spare_len) == -1)
		{//解析出一次命令
			break;
		}

		used_len += tmp_len;
		spare_len -= tmp_len;

		/* 把这个命令的处理结果追加到准备发送到服务器的buffer */
		create_ret_pack(msg_to_svr+total_len,cmd,option,&used_len1);

		total_len += used_len1;
	}
	*p_msglen = total_len;
}

static int send_by_line(int fd,char *send_buf,int count)
{
	int n_send;

	n_send = send(fd,send_buf,count,0);
	if(n_send < 0)
	{
		return -1;
	}

	return -1;
}

static int send_by_char(int fd,char *send_buf,int count)
{
	int n_send,total=0;

	while(count--)
	{
		n_send = send(fd,send_buf++,1,0);
		total += n_send;
		if(n_send < 0)
		{
			return -1;
		}
	}

	return total;
}

int Telnet_Send(char *send_buf,int count)
{
	int n_send;

	if(m_Tel_info.commit_mode == MODE_LINE)
	{
		n_send = send_by_line(m_Tel_info.fd,send_buf,count);
	}
	else if(m_Tel_info.commit_mode == MODE_CHAR)
	{
		n_send = send_by_char(m_Tel_info.fd,send_buf,count);
	}
	else
	{
		n_send = send_by_char(m_Tel_info.fd,send_buf,count);
	}

	return n_send;
}



int telnet_connect(char *server_ip,unsigned short server_port,tel_ctrl *tel_info)
{
	int sockfd,retval,len;
	struct sockaddr_in svr_addr;
	time_t pretime,nowtime;
    fd_set readfd;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        LOG("socket failed! error %ld\n", errno);
        return -1;
    }

	len = sizeof(svr_addr);
	memset(&svr_addr,0,len);
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(server_port);
	svr_addr.sin_addr.s_addr = inet_addr(server_ip);

#if (OS == WIN32)
    ioctlsocket(sockfd,FIONBIO,NULL);
#elif( OS==LINUX )
    fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFL) | O_NONBLOCK);
#endif

    //等待10s如果连接不上就失败
	pretime = time(NULL);
	do{
		retval = connect(sockfd,(struct sockaddr *)&svr_addr,len);
		nowtime = time(NULL);
		if((nowtime - pretime) > CONNECT_TIMEOUT)
		{
			retval = -1;
			break;
		}
        Task_Delay(1000);
	}while(retval == -1);

	if(retval != 0)
	{
		return -1;
	}

	tel_info->fd = sockfd;
	tel_info->connected = TRUE;

    FD_ZERO(&readfd);
    FD_SET(m_Tel_info.fd,&readfd);

	return 0;
}


int Telnet_Get(unsigned char *msg,int * plen)
{
	int n_recv;
    unsigned char * ptmpbuf = msg;

    *msg = '0';
    while(1)
    {
        int    i=0;
        int    msg_len;
        unsigned char msg_to_svr[128];

        if(!m_Tel_info.connected)
        {
            return -1;
        }

        n_recv = recv(m_Tel_info.fd,ptmpbuf,(*plen) - 1,0);
        if( -1 == n_recv )
        {
            Task_Delay(50);
            continue;
        }
        else if(n_recv < 0)
        {
            LOG("recv sock failed! error:%u",n_recv);
            return -1;
        }
        else if(n_recv == 0)
        {
            return -2;
        }

        ptmpbuf[n_recv] = '\0';
        if((ptmpbuf[i] == CMD_IAC) && (ptmpbuf[i+1] != CMD_IAC))
        {
            negotiate(ptmpbuf,msg_to_svr,n_recv,&msg_len);
            Telnet_Send((char *)msg_to_svr,msg_len);
        }
        else
        {
            *plen = n_recv + 1;
            break;
        }
    }

	return 0;
}

int Telnet_Run()
{
    return m_Tel_info.connected;
}

int Telnet_Init()
{
    int err;

#if (OS == WIN32)
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 1, 1 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        LOG("WSAStartup failed!\n");
        return -1;
    }
#endif

	m_Tel_info.commit_mode = MODE_LINE;
	m_Tel_info.connected = FALSE;
	m_Tel_info.fd = -1;

	err = telnet_connect(server_ip,server_port,&m_Tel_info);
	if(0 != err)
	{
		LOG("telnet connect ip (%s) port (%u) failed!\n",server_ip,server_port);
		return -1;
	}

	return 0;
}


int Telnet_Fini()
{
	m_Tel_info.connected = FALSE;
	if(m_Tel_info.fd)
	{
		shutdown(m_Tel_info.fd,2);
		close(m_Tel_info.fd);
	}

#if (OS == WIN32)
	if( 0 != WSACleanup())
	{
        LOG("telnet free not ok!\r\n");
        return -1;
	}
#endif

	return 0;
}

