//InfoFile.h�ļ��ľ������
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

	void writeRegister(char* name, char* pwd); //��ע����Ϣд�����
	void ReadRegister(CString &name, CString &pwd); //���ļ��ж�ȡ�û�������Ĳ���
}; 
