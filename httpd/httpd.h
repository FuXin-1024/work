/*************************************************************************
	> File Name: httpd.h
	> Author: fx
	> Mail: 1007160080qq.com 
	> Created Time: Fri 18 Aug 2017 07:43:26 AM PDT
 ************************************************************************/

#ifndef __HTTPD_H_
#define __HTTPD_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/sendfile.h>
#include<syslog.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<assert.h>
#include<ctype.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#define WARNING 1
#define FATAL 2
#define SIZE 102400

void print_log(const char* str,int error,int lines);
void send_error(int sock,const char* path);
void echo_error(int sock,int error_code);
int startup(const char* ip,const char* port);
int get_line(int sock,char* buf,int size);
void Dealwithsock(int sock);
int echo_cgi(int sock, char* method, char* path, char* query_string);
int echo_www(int sock,const char* path,int size);
void clear_header(int sock);
#endif

