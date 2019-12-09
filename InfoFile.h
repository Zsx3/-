//InfoFile.h文件的具体代码
#pragma once

#include <list>
#include <fstream>
#include <iostream>
#include <string>

#define F_LOGIN "./login.ini"

using namespace std;

class InfoFile
{
public:
	InfoFile();
	~InfoFile();

	void writeRegister(char* name, char* pwd); //将注册信息写入操作
	void ReadRegister(CString &name, CString &pwd); //从文件中读取用户名密码的操作
}; 
