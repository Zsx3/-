#include "stdafx.h"
#include "user_msg.h"
#include<iomanip>
user_msg::user_msg()
{
	
}


user_msg::~user_msg()
{
}

void user_msg::ReadDocline1()//将人员账号和密码载入一个list<user_information>方便索引查询用户对应的密码
{
	ifstream ifs(_F_USER);	//输入方式打开文件

	char buf[1024] = { 0 };
	user_list.clear();
	//取出表头（用户|密码）
	ifs.getline(buf, sizeof(buf));

	while (!ifs.eof())	//没到文件结尾
	{
		user_information tmp;//用于一次记录人员的账号和密码；通过push_back将所有人员账号密码放入

		ifs.getline(buf, sizeof(buf));	//读取一行
		//AfxMessageBox(CString(buf));
		char *sst = strtok(buf, "|");	//以"|"分隔
		if (sst != NULL)
		{
			tmp.name = CString(sst);	//用户名
		}
		else
		{
			break;
		}

		sst = strtok(NULL, "|");
		tmp.pwd = CString(sst);	//密码
		user_list.push_back(tmp);	//放在链表的后面
	}

	ifs.close();	//关闭文件
}

void user_msg::WriteDocline1(CString in_string, int index)
{
	ofstream ofs(_F_LOGIN_TIME, ios::app);//
	switch (index)
	{
	case 0://写入换行符
		ofs << endl;
		break;
	case 1://写入用户名
		ofs << setiosflags(ios::left) << setw(12) << in_string;
		//ofs << in_string<<"|";
		break;
	case 2://写入登录时间
		ofs << setiosflags(ios::left) << setw(40) << in_string;
		//ofs << in_string << "|";
		break;
	case 3://写入退出时间
		ofs << setiosflags(ios::left) << setw(40) << in_string;
		//ofs << in_string << "|";
		break;
	default:
		break;
	}
	ofs.close();
}
