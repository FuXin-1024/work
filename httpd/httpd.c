#include"httpd.h"

void print_log(const char* str,int error,int lines)
{
	char *err[2]= {"WARNING","FATAL"};
	openlog("print_log",LOG_CONS|LOG_PID,LOG_USER);

	syslog(LOG_INFO,"lines:%d %s %s\n",lines,str,err[error]);
	closelog();
}

void send_error(int sock,const char* path)
{
	if(path == NULL)
	{
		print_log("error page path is NULL---",FATAL,__LINE__);
		return;
	}
	struct stat st;
	if(stat(path,&st) < 0)
	{
		print_log("error page not found---",FATAL,__LINE__);
		return;
	}
	int fd = open(path,O_RDONLY);
	int ret = 0;
	if(ret = sendfile(sock,fd,0,st.st_size) < 0)
	{
		print_log("Error send error_page---",FATAL,__LINE__);
	}
}


void echo_error(int sock,int error_code)
{
	switch(error_code)
	{
	case 401:
		{
		char buf[100] = "HTTP/1.0 401 Unauthorized\r\n\r\n";
		send(sock,buf,strlen(buf),0);
		send_error(sock,"wwwroot/error/401.html");
		break;
		}
	case 400:
		{
		char buf[100] = "HTTP/1.0 400 Bad Request\r\n\r\n";
		send(sock,buf,strlen(buf),0);
		send_error(sock,"wwwroot/error/400.html");
		break;
		}
	case 403:
		{
		char buf[100] = "HTTP/1.0 403 Forbidden\r\n\r\n";
		send(sock,buf,strlen(buf),0);
		send_error(sock,"wwwroot/error/403.html");
		break;
		}
	case 404:
		{
		char buf[100] = "HTTP/1.0 404 Not Found\r\n\r\n";
		send(sock,buf,strlen(buf),0);
		send_error(sock,"wwwroot/error/404.html");
		break;
		}
	case 500:
		{
		char buf[100] = "HTTP/1.0 500 Internal Server Error\r\n\r\n";
		send(sock,buf,strlen(buf),0);
		send_error(sock,"wwwroot/error/500.html");
		break;
		}
	case 503:
		{
		char buf[100] = "HTTP/1.0 503 Server Unavailable\r\n\r\n";
		send(sock,buf,strlen(buf),0);
		send_error(sock,"wwwroot/error/503.html");
		break;
		}
	default:
    	break;
	}
}

int startup(const char* ip,const char* port)
{
	assert(ip);
	assert(port);

	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		print_log("socket error---",FATAL,__LINE__);
		exit(1);
	}

	int opt = 1;
	if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)) < 0)
	{
		print_log("setsockopt error---",WARNING,__LINE__);
	}

	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr=inet_addr(ip);
	int len = sizeof(addr);
	if(bind(sock,(struct sockaddr*)&addr,len) < 0)
	{
		print_log("bind error---",FATAL,__LINE__);
		close(sock);
		exit(2);
	}

	if(listen(sock,5) < 0)
	{
		print_log("listen error---",FATAL,__LINE__);
		close(sock);
		exit(3);
	}

	return sock;
}

int get_line(int fd,char* buf,int size)
{
	int i = 0;
	char c = '\0';
	while(i < (size -1) && (c != '\n'))
	{
		int n = recv(fd,&c,1,0);
		if(n > 0)
		{
			if( c == '\r')
			{
				int tmp = recv(fd,&c,1,MSG_PEEK);
				if(tmp > 0 && c == '\n')
				{
					recv(fd,&c,1,0);
				}
				else
				{
					c='\n';
				}
			}
			buf[i] = c;
			i++;
		}
		else
		{
			c = '\n';
		}
	}
	buf[i] = '\0';
	return i;
}

void clear_header(int sock)
{
	char buf[1024];
	int ret = 0;
	do{
		ret = get_line(sock,buf,sizeof(buf));
	}while(ret > 0 && strcmp("\n",buf));
}

int echo_www(int sock,const char* path,int size)
{
	int outfd = open(path,O_RDONLY);
	if(outfd < 0)
	{
		print_log("OPEN file(get out_fd) is error---",FATAL,__LINE__);
		return -1;
	}
	
	char buf[200] = "HTTP/1.0 200 OK\r\n\r\n";
	int headret = send(sock,buf,strlen(buf),0);
	if(headret <= 0)
	{
		echo_error(sock,500);
		print_log("ECHO head is error---",FATAL,__LINE__);
		return -1;

	}
	int ret= 0;
	if((ret = sendfile(sock,outfd,0,size)) < 0)
	{
		echo_error(sock,500);
		print_log("ECHO (CGI=0) is error---",FATAL,__LINE__);
		return -1;
	}
	return ret;
}

int echo_cgi(int sock, char* method, char* path, char* query_string)
{
	int content_len = -1;
	char method_env[SIZE/10];
	char query_string_env[SIZE];
	char content_len_env[SIZE/10];

	if(strcmp("GET",method) != 0)
	{ //POST

		printf("This is Post\n");
		char line[1024];
		int ret = -1;
		do{
			ret = get_line(sock,line,sizeof(line));
			if(ret > 0 && strncasecmp(line,"Content-Length: ",16) == 0){
				content_len = atoi(&(line[16]));
			}
		}while(ret > 0 && strcmp("\n",line));
	
		if(content_len <= 0){
			echo_error(sock,500);
			print_log("Content-Length is error---",FATAL,__LINE__);
			exit(-1);
		}
	}
	else
	{//GET
		clear_header(sock);
	}
		
	const char* echo_line="HTTP/1.0 200 OK \r\n";
	send(sock,echo_line,strlen(echo_line),0);
	const char* type="Content-Type:text/html;charset=ISO-8859-1\r\n";
	send(sock,type,strlen(type),0);
	const char* null_line="\r\n";
	send(sock,null_line,strlen(null_line),0);

	int input[2];
	int output[2];

	if(pipe(input)< 0 || pipe(output) < 0){
		echo_error(sock,500);
		print_log("pipe create error---",FATAL,__LINE__);
		close(sock);
		exit(-1);
	}
	pid_t id = 0;
	id = fork();
	if(id < 0){
		echo_error(sock,500);
		print_log("fork child error---",FATAL,__LINE__);
		close(sock);
		exit(-1);
	}
	else if(id == 0){
		close(input[1]);
		close(output[0]);

		sprintf(method_env,"METHOD=%s",method);
		putenv(method_env);

		if(!strcasecmp(method,"POST"))
		{
			sprintf(content_len_env,"CONTENT_LENGTH=%d",content_len);
			putenv(content_len_env);
		}
		if(!strcasecmp(method,"GET"))
		{
			sprintf(query_string_env,"QUERY_STRING=%s",query_string);
			putenv(query_string_env);
		}
	    
		dup2(input[0],0);
		dup2(output[1],1);
		execl(path,path,NULL);
	//	printf("execl weeoe!!!!!\n");
		exit(0);
	}
	else{
		close(input[0]);
		close(output[1]);

		if(strcasecmp("POST",method) == 0){
			char ch = '\0';
			int i = 0;
			for(i = 0;i < content_len;i++)
			{
				recv(sock,&ch,1,0);
				write(input[1],&ch,1);
			}
		}

		// get result
		 char ch = '\0';
        while(read(output[0],&ch,1) > 0)
		{
			send(sock,&ch,1,0);
		}

		waitpid(id,NULL,0);
		close(input[1]);
		close(output[0]);
	}
}

void Dealwithsock(int sock)
{
	int ret= 0;
	char buf[SIZE];
	char method[SIZE/10];
	char url[SIZE];
	char path[SIZE];
	int i,j;
	int cgi = 0;
	char* query_string = NULL;

	if(get_line(sock,buf,sizeof(buf)) <= 0)
	{

		echo_error(sock,400);
		print_log("getline is error---",FATAL,__LINE__);
		close(sock);
		exit(3);
	}

	i = 0; //method -> index
	j = 0; //buf -> index

	while(!isspace(buf[j]) &&\
			j < sizeof(buf)&&\
			i < sizeof(method)-1)
	{
		method[i] = buf[j];
		i++,j++;
	}
	method[i] =  0;

	if(strcasecmp(method,"GET") &&\
			strcasecmp(method,"POST"))
	{
		echo_error(sock,400);
		print_log("Method is error---",FATAL,__LINE__);
		close(sock);
		exit(3);
	}

	if(strcasecmp(method,"POST") == 0)
	{
		cgi = 1;
	}

//	buf-> "GET   /         http/1.0"
	while(isspace(buf[j]) && j<sizeof(buf))
		j++;

	i=0;
	while(!isspace(buf[j]) && j<sizeof(buf) && i<sizeof(url)-1)
	{
		url[i] = buf[j];
		i++,j++;
	}
	url[i] = 0;

//	printf("method: %s,url:%s\n",method,url);

	query_string = url;
	while(*query_string != '\0')
	{
		if(*query_string == '?')
		{
			*query_string = '\0';
			query_string++;
			cgi = 1;
			break;
		}
		query_string++;
	}
	sprintf(path,"wwwroot%s",url);
	if(path[strlen(path)-1] == '/')
	{
		strcat(path,"index.html");
	}

	struct stat st;
	if(stat(path,&st) != 0)
	{
		clear_header(sock);
		echo_error(sock,404);

		print_log("FILE is unknow---",FATAL,__LINE__);
		exit(4);
	}
	else
	{
		if(S_ISDIR(st.st_mode))
		{
			strcat(path,"/index.html");
		}
		else if((st.st_mode & S_IXUSR) ||\
				(st.st_mode & S_IXGRP) ||\
				(st.st_mode & S_IXOTH))
		{
			cgi = 1;
		}
		else
		{}

		if(cgi)
		{
		//	printf("enter CGI\n");
			echo_cgi(sock,method,path, query_string);
		}
		else
		{
		//	printf("method:%s,url:%s,path:%s,cgi:%d,query_string:%s\n",method,url,path,cgi,query_string);

			 clear_header(sock);
			 echo_www( sock, path,st.st_size);
		}
	}
//	printf("quit client.........\n");
	close(sock);
}



