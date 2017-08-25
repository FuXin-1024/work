#pragma once
#pragma warning(disable:4996)
#include<iostream>
#include<string>
#include<stdio.h>
using namespace std;

typedef long long INT64;    //long long 8字节
const INT64 UN_INIT = 0xcccccccccccccccc;
const long long MAX = 0x7fffffffffffffff;
const long long MIN = 0x8000000000000000;
#define MAXLEN  18

class BigData
{
public:

	BigData()
		:_value(UN_INIT)
	{}

	BigData(INT64 num)
		:_value(num)
	{
		char val[30];
		/*sprintf(val,"%d", num);*/
		_i64toa(num, val, 10);
		_strData = string(val);
	}
	BigData(const string& strData)
		:_value(0)
		, _strData("+0")
	{
		//去空串
		if (strData.empty())  //空串不做处理
		{
			return;
		}
		//去前面的空格；"   02316"          2316
		char *pData = (char*)strData.c_str();
		while (isspace(*pData))
		{
			pData++;
		}
		//"       "   处理完后，变为空串
		if (*pData == '\n')
			return;

		//处理符号
		char symbol;
		if (isdigit(*pData))
		{
			symbol = '+';
		}
		else if (*pData == '+' || *pData == '-')
		{
			symbol = *pData;
			pData++;
		}
		else
		{
			return;   //处理"abc123465"         0
		}

		//去掉前面的零
		while (*pData == '0')
		{
			pData++;
		}

		//全零处理
		if (*pData == '\0')
			return;

		//至此，所有字符串都转为正确的格式 "12365"  "ac23"  "1562aa" "135+362" "ac0000"-->"0" 均只处理为数字格式->"1256"
		int count = 1;
		_strData.resize(strlen(pData) + 1);
		_strData[0] = symbol;
		while (isdigit(*pData))
		{
			_value = _value * 10 + (*pData - '0');
			_strData[count++] = *pData;
			pData++;
		}
		//处理只剩下符号的情况的情况
		if (_strData == "+" || strData == "-")
		{
			_strData = "+0";
			return;
		}

		if (symbol == '-')
			_value = 0 - _value;


		//处理提取有效字符串部分；
		if (*pData == '\0')    //处理"000+123456"   到这一步剩下_strData = "+0" 而count为1，resize之后_strData变为"+";
			_strData.resize(count);
	}
	string GetStrData()
	{
		return _strData;
	}

	BigData operator+(const BigData& b)
	{
		//先判断是否超出范围，若没有则直接用四则运算来做，超出用字符转来运算
		if (IsINT64OverFlow() || b.IsINT64OverFlow())
		{
			//同号处理
			if (_strData[0] == b._strData[0])
				return BigData(StrAdd(_strData, b._strData));
			else //异号
				return BigData(StrSub(_strData, b._strData));
		}
		else//没有超出
		{	//同号
			if (_strData[0] == b._strData[0])//同号相加可能溢出
			{
				//MAX-左操作数 > 右操作数--->不会溢出
				if ((_strData[0] == '+' && MAX - _value > b._value) ||
					(_strData[0] == '-'&&MAX - _value < b._value))
				{
					//此时不会溢出
					return BigData(_value + b._value);
				}
				else
				{
					//相加后肯定不会溢出
					return StrAdd(_strData, b._strData);
				}
			}
			else
			{
				//异号，相加肯定不会溢出
				return BigData(_value + b._value);
			}
		}
	}

	BigData operator-(const BigData& b)
	{
		if (IsINT64OverFlow() || b.IsINT64OverFlow())//至少有一个超出范围
		{
			if (_strData[0] == b._strData[0])
			{
				return BigData(StrSub(_strData, b._strData));
			}
			else
			{
				return BigData(StrAdd(_strData, b._strData));
			}
		}
		else//没有超出范围，但是结果可能溢出
		{
			//同号相减一定不会溢出
			if (_strData[0] == b._strData[0])
			{
				long long ret = _value - b._value;
				return BigData(ret);
			}

			else
			{
				//MAX - | 左操作数 | >|右操作数|
				if (MAX - abs(_value) > abs(b._value))
					return BigData(_value - b._value);
				else
					return BigData(StrAdd(_strData, b._strData));
			}
		}
	}

	BigData operator*(const BigData& b)
	{
		return BigData(StrMul(_strData, b._strData));
		int Lleft = _strData.size();
		int Lright = b._strData.size();
		//相乘的结果不溢出，要求两个数的长度之和要小于机器最大能表示的长度
		if (Lleft + Lright - 2 < MAXLEN)
			return BigData(_value * b._value);
		else
		{
			//处理特殊情况  0* ，1*，-1*
			if (_strData == "+0" || b._strData == "+0")
			{
				//cout << 0;
				return BigData("+0");
			}

			if (_strData == "+1" || b._strData == "+1")
				return (_strData == "+1") ? BigData(b._strData) : BigData(_strData);
			if (_strData == "-1")
			{
				string tmp = b._strData;
				if (tmp[0] == '-')
				{
					tmp[0] = '+';
					return BigData(tmp);
				}
				else
				{
					tmp[0] = '-';
					return BigData(tmp);
				}
			}
			if (b._strData == "-1")
			{
				string tmp = _strData;
				if (tmp[0] == '-')
				{
					tmp[0] = '+';
					return BigData(tmp);
				}
				else
				{
					tmp[0] = '-';
					return BigData(tmp);
				}
			}
		}
		/*return BigData(StrMul(_strData, b._strData));*/
	}

	BigData operator/(const BigData& b)
	{
		if (b._strData == "+0")
		{
			cout << "除零错误\n" << endl;
			system("pause");
			exit(1);
		}

		//没有溢出情况下
		if (!IsINT64OverFlow() && !b.IsINT64OverFlow())
			return BigData(_value / b._value);

		//处理特殊情况，分子的绝对值比分母的绝对值小-->0, 0除，除正负1
		if (_strData == "+0")
			return BigData(0);
		if (b._strData == "+1")
			return BigData(_strData);
		if (b._strData == "-1")
		{
			string tmp(_strData);
			if (_strData[0] != b._strData[0])  //异号
				tmp[0] = '-';
			else
				tmp[0] = '+';

			return BigData(tmp);
		}
		//处理异常情况下的除操作
		int Lsize = _strData.size();
		int Rsize = b._strData.size();
		if (Lsize < Rsize)
			return BigData(0);
		else if (Lsize == Rsize)
		{
			if (_strData.compare(b._strData) == 0)
				return BigData(1);
			else
			{
				string left = _strData;
				string right = b._strData;
				if (_strData[0] != b._strData[0])  //将符号统一，进行比较
				{
					left[0] = '+';
					right[0] = '+';
				}

				if (left.compare(right) < 0)    //分子绝对值小于分母绝对值的情况
					return BigData(0);
			}
		}
		else
			return BigData(StrDiv(_strData, b._strData));

	}

	friend ostream& operator<<(ostream& _cout, const BigData& b)
	{
		char* data = (char *)b._strData.c_str();
		if ('+' == *data)
			++data;
		_cout << data;
		return _cout;
	}
protected:
	bool IsINT64OverFlow()const //判断是否溢出
	{
		BigData tmp("+9223372036854775807");
		if ('-' == _strData[0])
		{
			tmp._strData = "-9223372036854775808";
		}
		//判断长度
		if (_strData.size() > tmp._strData.size())
			return true;
		else if (_strData.size() == tmp._strData.size()) //长度相同，比较数值大小是否超出
		{
			if (_strData > tmp._strData)
				return true;
		}
		return false;
		//return true;
	}

	bool IsLeftBig(string left, string right) //左操作数 >=右操作数，返回true
	{
		if (left[0] == '-'&& right[0] == '+')
			return false;
		int Lsize = left.size();
		int Rsize = right.size();

		if (Lsize > Rsize)
			return true;
		else if (Lsize == Rsize)
		{
			if (left.compare(right) < 0)
				return false;
			else
				return true;
		}
		return false;
	}
	string StrAdd(string left, string right)//同号字符串相加
	{
		int Lleft = left.size();
		int Lright = right.size();
		//把长度长的作为左操作数
		if (Lleft < Lright)
		{
			left.swap(right);
			swap(Lleft, Lright);
		}

		//进行计算
		string strRes;
		strRes.resize(Lleft + 1); //考虑进位情况，可能进1位
		strRes[0] = left[0];//同号相加，符号位一定相同。
		char step = 0;		//进位

		for (int idx = 1; idx < Lleft; idx++)
		{
			char cRes = left[Lleft - idx] + step - '0';
			if (idx < Lright)
				cRes = cRes + (right[Lright - idx] - '0');
			step = 0;
			while (cRes >= 10) //逢十进位
			{
				step = 1;
				cRes -= 10;
			}
			strRes[Lleft - idx + 1] = cRes + '0';
		}
		//加上最后一次的进位
		strRes[1] = step + '0';
		return strRes;
	}

	string StrSub(string left, string right)
	{
		char symbol = '+';
		if (!IsLeftBig(left, right))
			symbol = '-';
		int Lleft = left.size();
		int Lright = right.size();
		//把大的字符串当左字符串
		if (Lleft <= Lright && !IsLeftBig(left, right))
		{
			left.swap(right);
			swap(Lleft, Lright);
		}
		//进行计算
		char ret = 0;//保存相减的结果
		for (int idx = 1; idx < Lleft; idx++)
		{
			char leftData = left[Lleft - idx] - '0';
			char rightData = right[Lright - idx] - '0';
			char src = left[Lleft - idx - 1];
			if (idx < Lright && leftData < rightData)//需要结尾 
			{
				left[Lleft - idx - 1] = src - 1;
				ret = leftData + 10 - rightData + '0';
				left[Lleft - idx] = ret;
			}
			else
			{
				if (idx < Lright)
					left[Lleft - idx] = leftData - rightData + '0';
				else
					break;
			}
		}
		left[0] = symbol;
		return left;
	}

	string StrMul(string left, string right)
	{
		int Lleft = left.size();
		int Lright = right.size();
		//长度长的操作数为左操作数
		if (Lleft < Lright)
		{
			left.swap(right);
			swap(Lleft, Lright);
		}
		string strRes;
		strRes.resize(1);   //这里为什么是1？  -->因为StrAdd时会根据两个操作数的长度重新设定大小(左右操作数长度 长的+1) 
		/*
		StrAdd("+" , "263") = "0263"
		若现在就计算出乘积之后的长度(假设5)，在StrAdd时：
		StrAdd("+0000","263") = "+00263" strAdd之后每次strRes的size会增加1;
		所以设置位1，让其随着StrAdd去增加位数；
		*/
		int len = strRes.size();
		strRes[0] = '+';

		/*连个字符串相乘，从低位依次取一个数与另一个数相称,然后把得到的字符串相加；
		结果存入字符串strRes中
		9999 * 99                    /strRes
		1.取第一位  9*99 = 891       /"+891" + "+0" = "+891"
		2.取次低位  9*99 = 891       /"+8910" + "+891" = "+9810"
		..
		最后求得乘积;
		*/

		int idx = 1;
		INT64 ret = 1;
		for (; idx < Lleft; idx++)
		{
			INT64 rslt = 0;
			string tmp = _mul(right, left[Lleft - idx]);//左操作数与右操作数的乘积
			for (size_t i = idx; i>1; i--)
			{
				tmp.append("0");
			}
			strRes = StrAdd(tmp, strRes); //两个字符串求和；此处的两个字符串一定是两个正数; / "+8910" + "+891" = "+9810"
		}
		if (left[0] != right[0])//两者异号
		{
			strRes[0] = '-';
		}
		return strRes;
	}

	string _mul(string left, char right)//右操作数为1位数，左操作数不限(只计算两个正整数的乘积)
	{
		if (right == '0')
			return "+0";

		int Lleft = left.size();
		int Lright = 1;

		string strRes;
		strRes.resize(Lleft + 1, '0');
		strRes[0] = '+';
		int idx;
		int step = 0;//进位
		for (idx = 1; idx < Lleft; idx++)
		{
			int num = (left[Lleft - idx] - '0')*(right - '0');
			step = num / 10;
			num = num % 10;

			strRes[Lleft - idx + 1] = (num)+(strRes[Lleft - idx + 1] - '0') + '0';
			strRes[Lleft - idx] = step + '0';
		}
		return strRes;
	}

	string StrDiv(string left, string right)
	{
		int Lleft = left.size();
		int Lright = right.size();

		string strRes;
		strRes.resize(Lleft);
		char symbol = '+';
		if (left[0] != right[0])//异号相除准为负
			symbol = '-';

		//符号统一
		left[0] = '+';
		right[0] = '+';

		int ret = 1;
		int idx = 1;
		string Ldata("+");
		for (; idx < Lleft; idx++)
		{
			Ldata.push_back(left[idx]);//将字符添加进去
			if (!IsLeftBig(Ldata, right))
			{
				strRes[ret++] = '0';
				continue;
			}
			int count = 0;
			while (1)
			{
				if (!IsLeftBig(Ldata, right))  //左操作数小
				{
					break;
				}
				count++;
				Ldata = StrSub(Ldata, right);  //此处一定是左操作数大,才进行计算
				Ldata = BigData(Ldata)._strData; //处理掉前面的零 "+096" -->"+96"
			}
			//走到这，确定了商的一位;
			strRes[ret++] = count + '0';
			//重置Ldata,准备下一次的计算
			//Ldata = BigData(Ldata)._strData;
			if (Ldata == "+0")  //处理掉，余数是零的情况，"+0" -->"+"  达到重置Ldata 的作用;
				Ldata.resize(1);
		}
		//符号处理;
		strRes[0] = symbol;
		return strRes;
	}

private:
	string _strData;//大数运算的数据
	long long _value;//没有超出范围的数
};