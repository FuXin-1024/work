/*************************************************************************
	> File Name: client.c
	> Author: fx
	> Mail: 1007160080qq.com 
	> Created Time: Thu 24 Aug 2017 07:57:48 PM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>


char name[30];

int Init(int port,const char* ip)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("socket");
		exit(-1);
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ip);
	int ret = connect(sock,(struct sockaddr*)&server,sizeof(server));
	if(ret < 0)
	{
		printf("connect error\n");
		perror("connect");
		exit(-2);
	}

	printf("客户端启动成功\n");
	return sock;
}

void* recv_thread(void* arg)
{
	int sock = *((int*)arg);
	while(1)
	{
		char buf[1024]={};
		if(recv(sock,buf,sizeof(buf),0)<= 0)
		{
			return;
		}
		printf("%s\n",buf);
	}
}
void Start(int port,const char* ip)
{

	int sock = Init(port,ip);

	pthread_t tid;
	int ret = pthread_create(&tid,NULL,recv_thread,&sock);
	if(ret < 0)
	{
		perror("pthread_create");
		exit(-1);
	}
	pthread_detach(tid);

	char buf2[1024]={};
	sprintf(buf2,"%s进入了聊天室",name);

	int ret_send_buf2 = send(sock,buf2,strlen(buf2),0);
	if(ret_send_buf2 <=0)
	{
		perror("send");
		exit(-2);
	}

	while(1)
	{
		char buf[1024]={};
		scanf("%s",buf);
		char msg[1024]={};
		char* quit="\\q";
		sprintf(msg,"%s:%s",name,buf);

	
		 int ret_send = send(sock,msg,strlen(msg),0);
		 if(ret_send <= 0 )
		 {
			perror("send");
			exit(-3);
		 }

		if(strcasecmp(buf,quit) == 0)
		{
			memset(buf2,0,sizeof(buf2));
			sprintf(buf2,"%s退出了聊天室",name);
			send(sock,buf2,strlen(buf2),0);
			break;
		}
	}
	close(sock);
}

void Usage(const char* name)
{
	printf("Usage is %s [IP] [PORT]\n",name);
}

int main(int argc,const char* argv[])
{
	if(argc != 3)
	{
		Usage(argv[0]);
		return 1;
	}
	printf("请输入昵称：");
	scanf("%s",name);
	Start(atoi(argv[2]),argv[1]);
	return 0;
}
