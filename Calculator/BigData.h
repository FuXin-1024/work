#pragma once
#pragma warning(disable:4996)
#include<iostream>
#include<string>
#include<stdio.h>
using namespace std;

typedef long long INT64;    //long long 8�ֽ�
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
		//ȥ�մ�
		if (strData.empty())  //�մ���������
		{
			return;
		}
		//ȥǰ��Ŀո�"   02316"          2316
		char *pData = (char*)strData.c_str();
		while (isspace(*pData))
		{
			pData++;
		}
		//"       "   ������󣬱�Ϊ�մ�
		if (*pData == '\n')
			return;

		//�������
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
			return;   //����"abc123465"         0
		}

		//ȥ��ǰ�����
		while (*pData == '0')
		{
			pData++;
		}

		//ȫ�㴦��
		if (*pData == '\0')
			return;

		//���ˣ������ַ�����תΪ��ȷ�ĸ�ʽ "12365"  "ac23"  "1562aa" "135+362" "ac0000"-->"0" ��ֻ����Ϊ���ָ�ʽ->"1256"
		int count = 1;
		_strData.resize(strlen(pData) + 1);
		_strData[0] = symbol;
		while (isdigit(*pData))
		{
			_value = _value * 10 + (*pData - '0');
			_strData[count++] = *pData;
			pData++;
		}
		//����ֻʣ�·��ŵ���������
		if (_strData == "+" || strData == "-")
		{
			_strData = "+0";
			return;
		}

		if (symbol == '-')
			_value = 0 - _value;


		//������ȡ��Ч�ַ������֣�
		if (*pData == '\0')    //����"000+123456"   ����һ��ʣ��_strData = "+0" ��countΪ1��resize֮��_strData��Ϊ"+";
			_strData.resize(count);
	}
	string GetStrData()
	{
		return _strData;
	}

	BigData operator+(const BigData& b)
	{
		//���ж��Ƿ񳬳���Χ����û����ֱ�������������������������ַ�ת������
		if (IsINT64OverFlow() || b.IsINT64OverFlow())
		{
			//ͬ�Ŵ���
			if (_strData[0] == b._strData[0])
				return BigData(StrAdd(_strData, b._strData));
			else //���
				return BigData(StrSub(_strData, b._strData));
		}
		else//û�г���
		{	//ͬ��
			if (_strData[0] == b._strData[0])//ͬ����ӿ������
			{
				//MAX-������� > �Ҳ�����--->�������
				if ((_strData[0] == '+' && MAX - _value > b._value) ||
					(_strData[0] == '-'&&MAX - _value < b._value))
				{
					//��ʱ�������
					return BigData(_value + b._value);
				}
				else
				{
					//��Ӻ�϶��������
					return StrAdd(_strData, b._strData);
				}
			}
			else
			{
				//��ţ���ӿ϶��������
				return BigData(_value + b._value);
			}
		}
	}

	BigData operator-(const BigData& b)
	{
		if (IsINT64OverFlow() || b.IsINT64OverFlow())//������һ��������Χ
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
		else//û�г�����Χ�����ǽ���������
		{
			//ͬ�����һ���������
			if (_strData[0] == b._strData[0])
			{
				long long ret = _value - b._value;
				return BigData(ret);
			}

			else
			{
				//MAX - | ������� | >|�Ҳ�����|
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
		//��˵Ľ���������Ҫ���������ĳ���֮��ҪС�ڻ�������ܱ�ʾ�ĳ���
		if (Lleft + Lright - 2 < MAXLEN)
			return BigData(_value * b._value);
		else
		{
			//�����������  0* ��1*��-1*
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
			cout << "�������\n" << endl;
			system("pause");
			exit(1);
		}

		//û����������
		if (!IsINT64OverFlow() && !b.IsINT64OverFlow())
			return BigData(_value / b._value);

		//����������������ӵľ���ֵ�ȷ�ĸ�ľ���ֵС-->0, 0����������1
		if (_strData == "+0")
			return BigData(0);
		if (b._strData == "+1")
			return BigData(_strData);
		if (b._strData == "-1")
		{
			string tmp(_strData);
			if (_strData[0] != b._strData[0])  //���
				tmp[0] = '-';
			else
				tmp[0] = '+';

			return BigData(tmp);
		}
		//�����쳣����µĳ�����
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
				if (_strData[0] != b._strData[0])  //������ͳһ�����бȽ�
				{
					left[0] = '+';
					right[0] = '+';
				}

				if (left.compare(right) < 0)    //���Ӿ���ֵС�ڷ�ĸ����ֵ�����
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
	bool IsINT64OverFlow()const //�ж��Ƿ����
	{
		BigData tmp("+9223372036854775807");
		if ('-' == _strData[0])
		{
			tmp._strData = "-9223372036854775808";
		}
		//�жϳ���
		if (_strData.size() > tmp._strData.size())
			return true;
		else if (_strData.size() == tmp._strData.size()) //������ͬ���Ƚ���ֵ��С�Ƿ񳬳�
		{
			if (_strData > tmp._strData)
				return true;
		}
		return false;
		//return true;
	}

	bool IsLeftBig(string left, string right) //������� >=�Ҳ�����������true
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
	string StrAdd(string left, string right)//ͬ���ַ������
	{
		int Lleft = left.size();
		int Lright = right.size();
		//�ѳ��ȳ�����Ϊ�������
		if (Lleft < Lright)
		{
			left.swap(right);
			swap(Lleft, Lright);
		}

		//���м���
		string strRes;
		strRes.resize(Lleft + 1); //���ǽ�λ��������ܽ�1λ
		strRes[0] = left[0];//ͬ����ӣ�����λһ����ͬ��
		char step = 0;		//��λ

		for (int idx = 1; idx < Lleft; idx++)
		{
			char cRes = left[Lleft - idx] + step - '0';
			if (idx < Lright)
				cRes = cRes + (right[Lright - idx] - '0');
			step = 0;
			while (cRes >= 10) //��ʮ��λ
			{
				step = 1;
				cRes -= 10;
			}
			strRes[Lleft - idx + 1] = cRes + '0';
		}
		//�������һ�εĽ�λ
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
		//�Ѵ���ַ��������ַ���
		if (Lleft <= Lright && !IsLeftBig(left, right))
		{
			left.swap(right);
			swap(Lleft, Lright);
		}
		//���м���
		char ret = 0;//��������Ľ��
		for (int idx = 1; idx < Lleft; idx++)
		{
			char leftData = left[Lleft - idx] - '0';
			char rightData = right[Lright - idx] - '0';
			char src = left[Lleft - idx - 1];
			if (idx < Lright && leftData < rightData)//��Ҫ��β 
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
		//���ȳ��Ĳ�����Ϊ�������
		if (Lleft < Lright)
		{
			left.swap(right);
			swap(Lleft, Lright);
		}
		string strRes;
		strRes.resize(1);   //����Ϊʲô��1��  -->��ΪStrAddʱ����������������ĳ��������趨��С(���Ҳ��������� ����+1) 
		/*
		StrAdd("+" , "263") = "0263"
		�����ھͼ�����˻�֮��ĳ���(����5)����StrAddʱ��
		StrAdd("+0000","263") = "+00263" strAdd֮��ÿ��strRes��size������1;
		��������λ1����������StrAddȥ����λ����
		*/
		int len = strRes.size();
		strRes[0] = '+';

		/*�����ַ�����ˣ��ӵ�λ����ȡһ��������һ�������,Ȼ��ѵõ����ַ�����ӣ�
		��������ַ���strRes��
		9999 * 99                    /strRes
		1.ȡ��һλ  9*99 = 891       /"+891" + "+0" = "+891"
		2.ȡ�ε�λ  9*99 = 891       /"+8910" + "+891" = "+9810"
		..
		�����ó˻�;
		*/

		int idx = 1;
		INT64 ret = 1;
		for (; idx < Lleft; idx++)
		{
			INT64 rslt = 0;
			string tmp = _mul(right, left[Lleft - idx]);//����������Ҳ������ĳ˻�
			for (size_t i = idx; i>1; i--)
			{
				tmp.append("0");
			}
			strRes = StrAdd(tmp, strRes); //�����ַ�����ͣ��˴��������ַ���һ������������; / "+8910" + "+891" = "+9810"
		}
		if (left[0] != right[0])//�������
		{
			strRes[0] = '-';
		}
		return strRes;
	}

	string _mul(string left, char right)//�Ҳ�����Ϊ1λ���������������(ֻ���������������ĳ˻�)
	{
		if (right == '0')
			return "+0";

		int Lleft = left.size();
		int Lright = 1;

		string strRes;
		strRes.resize(Lleft + 1, '0');
		strRes[0] = '+';
		int idx;
		int step = 0;//��λ
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
		if (left[0] != right[0])//������׼Ϊ��
			symbol = '-';

		//����ͳһ
		left[0] = '+';
		right[0] = '+';

		int ret = 1;
		int idx = 1;
		string Ldata("+");
		for (; idx < Lleft; idx++)
		{
			Ldata.push_back(left[idx]);//���ַ���ӽ�ȥ
			if (!IsLeftBig(Ldata, right))
			{
				strRes[ret++] = '0';
				continue;
			}
			int count = 0;
			while (1)
			{
				if (!IsLeftBig(Ldata, right))  //�������С
				{
					break;
				}
				count++;
				Ldata = StrSub(Ldata, right);  //�˴�һ�������������,�Ž��м���
				Ldata = BigData(Ldata)._strData; //�����ǰ����� "+096" -->"+96"
			}
			//�ߵ��⣬ȷ�����̵�һλ;
			strRes[ret++] = count + '0';
			//����Ldata,׼����һ�εļ���
			//Ldata = BigData(Ldata)._strData;
			if (Ldata == "+0")  //���������������������"+0" -->"+"  �ﵽ����Ldata ������;
				Ldata.resize(1);
		}
		//���Ŵ���;
		strRes[0] = symbol;
		return strRes;
	}

private:
	string _strData;//�������������
	long long _value;//û�г�����Χ����
};