#include<iostream>
#include<queue>
#include<stack>
#include<string>
#include"BigData.h"
using namespace std;
int Isp(char ch) //��ȡ��������ջ��������
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
			   cout << "û���ҵ��˲�����" << endl;
			   return -1;
	}
	}
}

int Icp(char ch) //��ȡ��ǰɨ�赽�����������ȼ���
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
			   cout << "û���ҵ��˲�����" << endl;
			   return -1;
	}
	}
}

//��׺ת��׺;
void PostFix(queue<string> &q, const string &s)
{
	char *pData = (char*)s.c_str();
	stack<char> symbol;   //��Ų�������ȷ�����ȼ�;
	symbol.push('#');
	string strRes;      //���ã���������ת
	while (1)
	{

		while ((*pData) >= '0' && (*pData) <= '9')  //������
		{
			strRes.push_back(*pData);
			pData++;
		}
		if (!strRes.empty())
		{
			q.push(strRes);  //��������ջ;
			strRes.clear();  //��գ��Ա��´β������ļ�¼;
		}

		//�ж���s���β
		if (*pData == '\n')
			break;
		//�ߵ��˴�pDataһ���ǲ�����
		if (*pData == ')')
		{
			//��ջ��Ԫ�ص�����ֱ������'('������ֹͣ;
			char ch;
			for (ch = symbol.top(); ch != '('; ch = symbol.top())
			{
				//����������strRes֮�������;
				strRes.push_back(ch);
				q.push(strRes);
				strRes.clear();

				symbol.pop();
			}
			symbol.pop();  //��ջ�е���'('
			pData++;
		}
		else if (!((*pData) >= '0' && (*pData) <= '9'))
		{
			char ch = symbol.top();
			if (Isp(ch) < Icp(*pData))  //*pData��������ջ�����ȼ� > ch��ջ�����ȼ���*pData�������ڱ��ʽ��������ִ�У���ջ;
			{
				symbol.push(*pData);
			}
			else     //*pData��������ջ�����ȼ� < ch��ջ�����ȼ�  ,ch��������ִ�У���ջ����У�ֱ��*pData�������Լ�С�Ĳ���ջ,*pData���ȼ�����ջ�������Ԫ�أ�
			{
				while (Isp(ch) > Icp(*pData))
				{
					strRes.push_back(ch);
					q.push(strRes);
					strRes.clear();

					symbol.pop();
					ch = symbol.top();
				}

				//�˴�*pData��������ջ
				symbol.push(*pData);
			}

			pData++;
		}
	}

	//����ջ�еĲ�����
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

//��׺���ʽ�ļ���
string CountRPN(queue<string> &q)
{
	stack<string> strRes;  //������������ͼ���֮��Ľ��
	string str = q.front();

	while (!q.empty())
	{
		if (str == "+" || str == "-" || str == "*" || str == "/")
		{
			DoOperator(strRes, str[0]);
		}
		else   //�˴�str�ǲ�����
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

bool IsRightStr(const string &s)   //������ʽ��ȷ����true
{
	char *err1 = "(-";
	char *err2 = "+-";
	if (s.find(err1, 0) == string::npos && s.find(err2, 0) == string::npos)
		return true;

	return false;
}


void Usage1()    //���ʽ�������
{
	cout << "Usage:" << endl;
	cout << "###############################################################" << endl;
	cout << "example:1+2-3*4+5/6+(-1*2)" << endl;
	cout << "Usage:1+2-3*4+5/6((0-1)*2)" << endl;
	cout << "ʹ�ø���ʱ-1�����ʽΪ(0-1)��Ϊ����������" << endl;;
	cout << "###############################################################" << endl;
	cout << endl;
}

//void CalMenu()
//{
//	cout << "###############################################################" << endl;
//	cout << "####################   ---��ӭʹ�ü�����---     ###############" << endl;
//	cout << "####################    1.��ʼ����              ###############" << endl;
//	cout << "####################    2.ʹ�÷���              ###############" << endl;
//	cout << "####################    0.�˳�                  ###############" << endl;
//	cout << "###############################################################" << endl;
//}
void Calcul()
{
	cin.sync();
	queue<string> q;
	cout << "�����������ʽ��";

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
		printf("���������ʽ��������������\n");
		Usage1();
	}
}



