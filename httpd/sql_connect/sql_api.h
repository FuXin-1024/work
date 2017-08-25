/*************************************************************************
	> File Name: sql_api.h
	> Author: fx
	> Mail: 1007160080qq.com 
	> Created Time: Mon 21 Aug 2017 06:21:57 AM PDT
 ************************************************************************/

#ifndef _SQL_
#define _SQL_

#include<iostream>
#include<string>
#include<mysql.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
using namespace std;

class sqlApi
{
public:
	sqlApi(const string &_h,\
			const int &_port,\
			const string &_u="root",\
			const string &_p="root",\
			const string &db="myhttp");

	int connect();
	int insert(const string &_name,\
			const string &_sex,\
			const string &_age,\
			const string &_hobby,\
			const string &_school);

	int select();
	int Delete(const string& _id);
	int updata(const string &_name,\
	   const string &_age,\
	   const string &_sex,\
	   const string &_hobby,\
	   const string &_school,\
	   const string &_id);

	~sqlApi();

private:
	MYSQL* conn;
	MYSQL_RES* res;
	string host;
	string user;
	string passwd;
	string db;
	int port;
};

#endif
