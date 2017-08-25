/*************************************************************************
	> File Name: sql_api.cpp
	> Author: fx
	> Mail: 1007160080qq.com 
	> Created Time: Mon 21 Aug 2017 06:21:57 AM PDT
 ************************************************************************/

#include"sql_api.h"

sqlApi::sqlApi(const string &_h,\
	   const int &_port,\
	   const string &_u,\
	   const string &_p,\
	   const string &_db)
{
	host = _h;
	user = _u;
	passwd = _p;
	db = _db;
	port = _port;
	res = NULL;
	conn = mysql_init(NULL);
}

int sqlApi::connect()
{
	if(mysql_real_connect(conn,host.c_str(),user.c_str(),passwd.c_str(),\
				db.c_str(),port,NULL,0))
	{
		cout<<"connect success!"<<endl;
		printf("<br>");

	}
	else
	{
		cout<<"connect error!"<<endl;
		printf("<br>");
	}
}


int sqlApi:: insert(const string &_name,\
		const string &_sex,\
		const string &_age,\
		const string &_hobby,\
		const string &_school)
{
	string sql = "INSERT INTO student (name,sex,age,hobby,school) VALUES ('";
	sql += _name;
	sql += "','";

	sql += _sex;
	sql += "','";

	sql += _age;
	sql += "','";
	
	sql += _hobby;
	sql += "','";

	sql += _school;
	sql += "')";

	cout<<sql<<endl;
	printf("<br>");
	int ret = mysql_query(conn,sql.c_str());
	if(ret != 0)
	{
		cout<<"insert error!"<<endl;
		printf("<br>");
	}
}


int sqlApi:: select()
{
	string sql = "select * from student";
	if(mysql_query(conn,sql.c_str()) == 0)
	{
		res = mysql_store_result(conn);
		if(res)
		{
			int nums = mysql_num_rows(res);
			int col = mysql_num_fields(res);
			cout<<"nums: "<<nums<<" col: "<<col<<endl;
			printf("<br>");
			MYSQL_FIELD *fd;
			for(;fd = mysql_fetch_field(res);)
			{
				cout<<fd->name<<"   ";
			}
			cout<<endl;
			printf("<br>");

			int i = 0;
			int j = 0;
			for(; i<nums; i++)
			{
				MYSQL_ROW row_res = mysql_fetch_row(res);
				j = 0;
				for(; j<col; j++)
				{
					cout<<row_res[j];
					printf("&nbsp");
					printf("&nbsp");
				}
				cout<<endl;
				printf("<br>");
			}
			cout<<endl;
			printf("<br>");
		}
	}
}

int sqlApi::Delete(const string& _id)
{
	string sql = "DELETE FROM student where id=";
	sql +=_id;

	cout<<sql<<endl;
	printf("<br>");

	int ret = mysql_query(conn,sql.c_str());
	if(ret != 0)
	{
		cout<<"delete error!"<<endl;
		printf("<br>");
	}

}


int sqlApi::updata(const string &_name,\
	   const string &_age,\
	   const string &_sex,\
	   const string &_hobby,\
	   const string &_school,\
	   const string &_id)
{
	string sql = "UPDATE student SET ";
	sql +="name='";
	sql +=_name;
	sql +="',";


	sql +="age=";
	sql +=_age;
	sql +=",";

	sql +="sex='";
	sql +=_sex;
	sql +="',";

	sql +="hobby='";
	sql +=_hobby;
	sql +="',";

	sql +="school='";
	sql +=_school;
	sql +="' WHERE id=";
	sql +=_id;
    //sql +=";";
	cout<<sql;
	printf("<br>");

	int ret = mysql_query(conn,sql.c_str());
	if(ret != 0)
	{
		cout<<"updata error!"<<endl;
		printf("<br>");
	}
}
sqlApi::~sqlApi()
{
	mysql_close(conn);
}

