/*************************************************************************
	> File Name: cgifun.c
	> Author: fx
	> Mail: 1007160080qq.com 
	> Created Time: Sat 19 Aug 2017 01:58:48 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>

void mymath(char* arg)
{
	printf("<html><body><h1>");
	printf("Hello World!");
	printf("</h1></body></html>");
}
int main()
{
	printf("<html><body><h1>");
	printf("Hello World!");
	printf("This is cgifun!\n");
	printf("</h1></body></html>");

	char *method = NULL;
	char *query_string = NULL;
	char *string_arg = NULL;
	int content_len = -1;
	char buf[1024];

	if((method=getenv("METHOD")))
	{
		if(strcasecmp(method,"GET") == 0)
		{
			if((query_string = getenv("QUERY_STRING")))
			{
				string_arg = query_string;
			}
		}
		else
		{
			if(getenv("CONTENT_LENGTH"))
			{
				content_len = atoi(getenv("CONTENT_LENGTH"));
				int i = 0;
				for(; i < content_len; i++)
				{
					read(0,&buf[i],1);
				}
				buf[i]='\0';
				string_arg = buf;
			}
		}
	}
	printf("%s\n",string_arg);
	mymath(string_arg);
	
}
