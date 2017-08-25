#include<iostream>
#include<queue>
#include<stack>
#include<string>
#include"BigData.h"
using namespace std;
int Isp(char ch) //获取操作符进栈后优先数
{
	switch (ch)
	{
	case '#':
		return 0;
	case '(':
		return 1;

	case '+':
	case '-':
		return 3;

	case '*':
	case '/':
		return 5;

	case ')':
		return 8;
	default:
	{
			   cout << "没有找到此操作符" << endl;
			   return -1;
	}
	}
}

int Icp(char ch) //获取当前扫描到操作符的优先级数
{
	switch (ch)
	{
	case '#':
		return 0;
	case '(':
		return 8;

	case '+':
	case '-':
		return 2;

	case '*':
	case '/':
		return 4;

	case ')':
		return 1;
	default:
	{
			   cout << "没有找到此操作符" << endl;
			   return -1;
	}
	}
}

//中缀转后缀;
void PostFix(queue<string> &q, const string &s)
{
	char *pData = (char*)s.c_str();
	stack<char> symbol;   //存放操作数，确定优先级;
	symbol.push('#');
	string strRes;      //作用：操作数中转
	while (1)
	{

		while ((*pData) >= '0' && (*pData) <= '9')  //操作数
		{
			strRes.push_back(*pData);
			pData++;
		}
		if (!strRes.empty())
		{
			q.push(strRes);  //操作数入栈;
			strRes.clear();  //清空，以便下次操作数的记录;
		}

		//判断是s否结尾
		if (*pData == '\n')
			break;
		//走到此处pData一定是操作符
		if (*pData == ')')
		{
			//将栈的元素弹出，直到遇到'('弹出并停止;
			char ch;
			for (ch = symbol.top(); ch != '('; ch = symbol.top())
			{
				//操作数借用strRes之手入队列;
				strRes.push_back(ch);
				q.push(strRes);
				strRes.clear();

				symbol.pop();
			}
			symbol.pop();  //从栈中弹出'('
			pData++;
		}
		else if (!((*pData) >= '0' && (*pData) <= '9'))
		{
			char ch = symbol.top();
			if (Isp(ch) < Icp(*pData))  //*pData操作符的栈外优先级 > ch的栈内优先级，*pData操作符在表达式运算中先执行，入栈;
			{
				symbol.push(*pData);
			}
			else     //*pData操作符的栈外优先级 < ch的栈内优先级  ,ch操作符先执行，出栈入队列，直到*pData遇到比自己小的才入栈,*pData优先级大于栈中下面的元素；
			{
				while (Isp(ch) > Icp(*pData))
				{
					strRes.push_back(ch);
					q.push(strRes);
					strRes.clear();

					symbol.pop();
					ch = symbol.top();
				}

				//此处*pData操作符入栈
				symbol.push(*pData);
			}

			pData++;
		}
	}

	//清理栈中的操作符
	while (symbol.top() != '#')
	{
		strRes.push_back(symbol.top());
		q.push(strRes);
		strRes.clear();

		symbol.pop();
	}
}
void DoOperator(stack<string> &strRes, const char ch)
{
	BigData right = strRes.top();
	strRes.pop();
	BigData left = strRes.top();
	strRes.pop();

	BigData rel;
	switch (ch)
	{
	case '+':
		rel = (left + right);
		//strRes.push((left + right).GetStrData());
		break;
	case '-':
		rel = (left - right);
		//strRes.push((left - right).GetStrData());
		break;
	case '*':
		rel = (left * right);
		//strRes.push((left * right).GetStrData());
		break;
	case '/':
		rel = (left / right);
		//strRes.push((left / right).GetStrData());
		break;
	default:
		break;
	}

	strRes.push(rel.GetStrData());
}

//后缀表达式的计算
string CountRPN(queue<string> &q)
{
	stack<string> strRes;  //保存操作数，和计算之后的结果
	string str = q.front();

	while (!q.empty())
	{
		if (str == "+" || str == "-" || str == "*" || str == "/")
		{
			DoOperator(strRes, str[0]);
		}
		else   //此处str是操作数
		{
			strRes.push(str);
		}
		q.pop();
		if (!q.empty())

			str = q.front();
	}

	return strRes.top();
}

string GetString()
{
	string str;
	char ch = getchar();
	while (ch != '\n')
	{
		if (!isspace(ch))
			str.push_back(ch);

		ch = getchar();
	}
	str.push_back('\n');
	return str;
}

bool IsRightStr(const string &s)   //输入表达式正确返回true
{
	char *err1 = "(-";
	char *err2 = "+-";
	if (s.find(err1, 0) == string::npos && s.find(err2, 0) == string::npos)
		return true;

	return false;
}


void Usage1()    //表达式输入规则
{
	cout << "Usage:" << endl;
	cout << "###############################################################" << endl;
	cout << "example:1+2-3*4+5/6+(-1*2)" << endl;
	cout << "Usage:1+2-3*4+5/6((0-1)*2)" << endl;
	cout << "使用负数时-1，其格式为(0-1)作为负数的输入" << endl;;
	cout << "###############################################################" << endl;
	cout << endl;
}

//void CalMenu()
//{
//	cout << "###############################################################" << endl;
//	cout << "####################   ---欢迎使用计算器---     ###############" << endl;
//	cout << "####################    1.开始计算              ###############" << endl;
//	cout << "####################    2.使用方法              ###############" << endl;
//	cout << "####################    0.退出                  ###############" << endl;
//	cout << "###############################################################" << endl;
//}
void Calcul()
{
	cin.sync();
	queue<string> q;
	cout << "请输入计算表达式：";

	string str = GetString();

	//cout << str << " = ";
	if (IsRightStr(str))
	{
		PostFix(q, str);
		string rel = CountRPN(q);

		str.pop_back();
		cout << str << " = " << BigData(rel) << endl;
	}
	else
	{
		printf("您的输入格式有误，请重新输入\n");
		Usage1();
	}
}



