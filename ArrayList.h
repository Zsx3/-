#include "stdafx.h"
class CallmfcuiDlg;

class CClientItem
{
public:
	CString m_strIp;
	SOCKET m_ClientSocket;
	HANDLE m_hThread;
	CallmfcuiDlg *m_pMainWnd;
	CClientItem(){
		m_ClientSocket = INVALID_SOCKET;
		m_hThread = NULL;
		m_pMainWnd = NULL;
	}
};