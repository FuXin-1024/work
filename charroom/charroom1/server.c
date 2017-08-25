/*************************************************************************
	> File Name: server.c
	> Author: fx
	> Mail: 1007160080qq.com 
	> Created Time: Thu 24 Aug 2017 07:18:14 PM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

//int ListenSock;//监听套接字
int fds[100];//客户端socketfd,100个元素
int size=2;
char* IP="127.0.0.1";
short PORT=8080;

int StartUp(int port,const char* ip)
{
	int	ListenSock = socket(AF_INET,SOCK_STREAM,0);
	if(ListenSock == 1)
	{
		perror("socket");
		exit(-1);
	}
	int opt = 1;
	setsockopt(ListenSock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);

	if(bind(ListenSock,(struct sockaddr*)&local,sizeof(local)) < 0)
	{
		perror("bind");
		exit(-2);
	}
	if(listen(ListenSock,5) < 0)
	{
		perror("listen");
		exit(-3);
	}
	return ListenSock;
}

void SendMsgToAll(char* msg)
{
//	printf("This is SendMsgToAll\n");
	int i = 0;
	for(i = 0; i<size; i++)
	{
		if(fds[i] != 0)
		{
			printf("send to %d\n",fds[i]);
			send(fds[i],msg,strlen(msg),0);
		}
	}
}

void* server_thread(void* arg)
{
	int fd = *(int*)arg;
	printf("ptrhread  = %d\n",fd);;
	while(1)
	{
		char buf[1024]={};
		if(recv(fd,buf,sizeof(buf),0) <= 0)
		{
			int i;
			for(i=0; i<size; i++)
			{
				if(fd == fds[i])
				{
					fds[i] = 0;
					break;
				}
			}
			printf("退出: fd=%d quit\n",fd);
			pthread_exit((void*)i);
		}
		SendMsgToAll(buf);
	}
}

void Service(int tmp)
{
	printf("服务器启动\n");
	while(1)
	{
     	int listenSock = tmp;
	    struct sockaddr_in client;
      	int len = sizeof(client);
		int sock = accept(listenSock,(struct sockaddr*)&client,&len);
		if(sock < 0)
		{
			perror("accept");
			continue;
		}

		int i = 0;
		for(i=0; i<size;i++)
		{
			if(fds[i] == 0)
			{
				//记录客户端的socket
				fds[i]= sock;
				printf(" sock = %d\n",sock);
				//客户端连接后，启动线程给此客户服务
				pthread_t tid;
				int ret=pthread_create(&tid,NULL,server_thread,&sock);
				if(ret < 0)
				{
					perror("pthread_create");
					exit(-1);
				}
				pthread_detach(tid);
				break;
			}
		}
			if(size == i)
			{
				char str[]={"对不起，聊天室已经满了!"};
				int ret_send=send(sock,str,strlen(str),0);
				if(ret_send <= 0)
				{
					perror("send");
					exit(-2);
				}
				close(sock);
			}
		
	}
}

void Usage(const char* name)
{
	printf("Usage is %s [IP] [PORT]\n",name);
}

int main(int argc,char* argv[])
{
	if(argc != 3)
	{
		Usage(argv[0]);
		return 1;
	}
	int tmp = StartUp(atoi(argv[2]),argv[1]);
	Service(tmp);
	return 0;
}
