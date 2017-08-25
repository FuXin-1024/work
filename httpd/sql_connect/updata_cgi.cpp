/*************************************************************************
	> File Name: updata_cgi.cpp
	> Author: fx
	> Mail: 1007160080qq.com 
	> Created Time: Mon 21 Aug 2017 06:55:01 AM PDT
 ************************************************************************/
#include"sql_api.h"

void updataToDb(const char* string_arg)
{
	cout<<string_arg;
	printf("<br>");
	char arg_buff[1024];
	//name="a"&sex="man"&arg="12"&......
	char *myargv[7];
	strcpy(arg_buff,string_arg);
	char *start = arg_buff;
	int i = 0;
	while(*start)
	{
		if(*start == '=')
		{
			start++;
			myargv[i++] = start;
			continue;
		}
		if(*start == '&')
		{
			*start = 0;
		}
		start++;
	}
	myargv[i] = NULL;
	sqlApi mydb("127.0.0.1",3306);
	mydb.connect();
	mydb.updata(myargv[0],myargv[2],myargv[1],myargv[3],myargv[4],myargv[5]);
}
int main()
{

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
			if(getenv("CONNECT_LENGTH"))
			{
				content_len = atoi(getenv("CONTENT_LENGTH"));
				int  i = 0;
				for(; i<content_len; i++)
				{
					read(0,&buf[i],1);
				}
				buf[i] = '\0';
				string_arg = buf;
			}
		}
	}
	
	updataToDb(string_arg);



	/*cout<<"This is insert_cgi!"<<endl;	
	sqlApi mydb("127.0.0.1",3306);
	mydb.connect();
	mydb.insert("zhangsan","man","18","play","sust");
	*/
	return 0;
}



