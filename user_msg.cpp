#include "stdafx.h"
#include "user_msg.h"
#include<iomanip>
user_msg::user_msg()
{
	
}


user_msg::~user_msg()
{
}

void user_msg::ReadDocline1()//����Ա�˺ź���������һ��list<user_information>����������ѯ�û���Ӧ������
{
	ifstream ifs(_F_USER);	//���뷽ʽ���ļ�

	char buf[1024] = { 0 };
	user_list.clear();
	//ȡ����ͷ���û�|���룩
	ifs.getline(buf, sizeof(buf));

	while (!ifs.eof())	//û���ļ���β
	{
		user_information tmp;//����һ�μ�¼��Ա���˺ź����룻ͨ��push_back��������Ա�˺��������

		ifs.getline(buf, sizeof(buf));	//��ȡһ��
		//AfxMessageBox(CString(buf));
		char *sst = strtok(buf, "|");	//��"|"�ָ�
		if (sst != NULL)
		{
			tmp.name = CString(sst);	//�û���
		}
		else
		{
			break;
		}

		sst = strtok(NULL, "|");
		tmp.pwd = CString(sst);	//����
		user_list.push_back(tmp);	//��������ĺ���
	}

	ifs.close();	//�ر��ļ�
}

void user_msg::WriteDocline1(CString in_string, int index)
{
	ofstream ofs(_F_LOGIN_TIME, ios::app);//
	switch (index)
	{
	case 0://д�뻻�з�
		ofs << endl;
		break;
	case 1://д���û���
		ofs << setiosflags(ios::left) << setw(12) << in_string;
		//ofs << in_string<<"|";
		break;
	case 2://д���¼ʱ��
		ofs << setiosflags(ios::left) << setw(40) << in_string;
		//ofs << in_string << "|";
		break;
	case 3://д���˳�ʱ��
		ofs << setiosflags(ios::left) << setw(40) << in_string;
		//ofs << in_string << "|";
		break;
	default:
		break;
	}
	ofs.close();
}
