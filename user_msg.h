#pragma once
#include <list>
#include <fstream>
#include <iostream>

#define _F_USER "./user.txt"
#define  _F_LOGIN_TIME "./log_time.txt"
using namespace std;

struct user_information
{
	CString name;
	CString pwd;
	CString m_time_login;
	CString m_time_logout;

};

class user_msg
{
public:
	user_msg();
	~user_msg();
	void ReadDocline1();
	void WriteDocline1(CString in_string, int index);
	list<user_information> user_list;
};
