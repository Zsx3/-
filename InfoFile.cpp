//InfoFile.cpp�ľ������
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
	ifs.open(F_LOGIN); //���ļ�

	char buf[1024] = { 0 };

	ifs.getline(buf, sizeof(buf));//��ȡ�û�����Ϣ
	name = CString(buf);

	ifs.getline(buf, sizeof(buf));//��ȡ������Ϣ
	pwd = CString(buf);

	ifs.close();
}

void InfoFile::writeRegister(char* name, char* pwd)
{
	ofstream ofs;
	ofs.open(F_LOGIN);

	ofs << name << endl;//д���û�����Ϣ
	ofs << pwd << endl;//д��������Ϣ
	ofs.close();
}