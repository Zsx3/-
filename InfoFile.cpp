//InfoFile.cpp的具体代码
#include "stdafx.h"
#include "InfoFile.h"
InfoFile::InfoFile()
{
}

InfoFile::~InfoFile()
{
}
void InfoFile::ReadRegister(CString &name, CString &pwd)
{
	ifstream ifs;
	ifs.open(F_LOGIN); //打开文件

	char buf[1024] = { 0 };

	ifs.getline(buf, sizeof(buf));//读取用户名信息
	name = CString(buf);

	ifs.getline(buf, sizeof(buf));//读取密码信息
	pwd = CString(buf);

	ifs.close();
}

void InfoFile::writeRegister(char* name, char* pwd)
{
	ofstream ofs;
	ofs.open(F_LOGIN);

	ofs << name << endl;//写入用户名信息
	ofs << pwd << endl;//写入密码信息
	ofs.close();
}