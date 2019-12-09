// CLoginDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "all_mfc_ui.h"
#include "CLoginDlg.h"
#include "afxdialogex.h"
#include "InfoFile.h"
#include "user_msg.h"
// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)


CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(DIALOG_LOGIN, pParent)
	, m_user(_T(""))
	, m_pwd(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_user);
	DDX_Text(pDX, IDC_EDIT_PWD, m_pwd);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_LOGIN_BTN, &CLoginDlg::OnBnClickedLoginBtn)
	ON_BN_CLICKED(IDC_CANCLE_LOGIN_BTN, &CLoginDlg::OnBnClickedCancleLoginBtn)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序




void CLoginDlg::OnBnClickedLoginBtn()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);//控件内容同步到变量上
	if (m_user.IsEmpty() || m_pwd.IsEmpty())
	{
		MessageBox(TEXT("输入内容不能为空"));
		return;
	}

	//获取正确的值
	user_information login_ifm;
	user_msg file;
	//CString name, pwd;
	file.ReadDocline1();//将所有的账户人员账号和密码放入到list<user_information>
	for (list<user_information>::iterator it = file.user_list.begin(); it != file.user_list.end(); it++)
	{
		if (it->name == m_user && it->pwd == m_pwd)
		{
			CDialog::OnCancel();
			CTime temp_time;
			temp_time = CTime::GetCurrentTime();             //获取当前时间日期
			CString chinese_expreesion = temp_time.Format("%w");//星期转换为中文的表达方式
			int p = atoi(chinese_expreesion);
			//int p = 1;
			char* week = 0;
			switch (p)
			{
			case 0:
				week = "星期日";
			case 1:
				week = "星期一";
			case 2:
				week = "星期二";
			case 3:
				week = "星期三";
			case 4:
				week = "星期四";
			case 5:
				week = "星期五";
			case 6:
				week = "星期六";
			default:
				break;
			}
			login_ifm.m_time_login = temp_time.Format(_T("%Y-%m-%d %H:%M:%S")) + " " + CString(week);
			MessageBox(login_ifm.m_time_login);
			file.WriteDocline1(m_user, 0);
			file.WriteDocline1(m_user, 1);
			file.WriteDocline1(login_ifm.m_time_login, 2);

			current_user_name = m_user;//记录当前登入人员的id
			return;
		}
	}
	MessageBox(TEXT("用户名或密码错误"));
	return;
}


void CLoginDlg::OnBnClickedCancleLoginBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(0);
}


void CLoginDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void CLoginDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//关闭当前模态框
	//CDialogEx::OnClose();

	//改为退出程序
	exit(0);
}

//char* CLoginDlg::transform_function_login(CString str)
//{
//	int n = str.GetLength(); //按字符计算，str的长度
//
//	int len = WideCharToMultiByte(CP_ACP, 0, str, n, NULL, 0, NULL, NULL);//按Byte计算str长度
//
//	char *pChStr = new char[len + 1];//按字节为单位
//
//	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength() + 1, pChStr, len + 1, NULL, NULL);
//
//	pChStr[len+1] = '/0';
//	return pChStr;
//}
