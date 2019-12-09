
// all_mfc_uiDlg.h: 头文件
//

#pragma once
#include <opencv2/opencv.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core.hpp>
#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include<WinSock2.h>
#include<stdio.h>
#include <vector>
#include "ArrayList.h"
#pragma comment(lib,"ws2_32.lib")

#include<cstdlib>
#include<ctime>

#include "png_pic.h"
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

using namespace cv;
// CallmfcuiDlg 对话框
BOOL socket_Select(SOCKET hSocket, DWORD nTimeOut, BOOL bRead);
DWORD WINAPI ListenThreadFunc(LPVOID Lparam);
DWORD WINAPI ClientThreadProc(LPVOID Lparam);
CString GetTime();

class CallmfcuiDlg : public CDialogEx
{
// 构造
public:
	CallmfcuiDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALL_MFC_UI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	BOOL StartServer(void);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedpaintrectbtn();
	afx_msg void OnBnClickedpaintpointbtn();
	afx_msg void OnBnClickedclearbtn();
	afx_msg void OnBnClickedsurebtn();
	afx_msg void OnBnClickeddatadownloadbtn();
	afx_msg void OnBnClickedexitbtn();
	afx_msg void OnClose();
	afx_msg void OnBnClickedreadpic();	
	afx_msg void OnBnClickedButtonsend();
	afx_msg void OnBnClickedButtonend();	
	afx_msg void OnBnClickedButtonstart();
	CButton m_paint_rect;
	CButton m_paint_point;
	CButton m_clear;
	CButton m_sure;
	CButton m_datadownload;
	CButton m_start;
	CButton m_server_start;
	CButton m_server_end;	

//socket
public:
	SOCKET m_SockListen;
	uint m_ServicePort;
	BOOL m_isServerOpen;
	HANDLE m_hListenThread;
	CArray<CClientItem> m_ClientArray;	
	CEdit m_EditRevBox;
public:
	BOOL equal(const CClientItem *p1, const CClientItem * p2);
	void OnEnChangeEditsendbox();
	void SendClientMsg(CString strMsg, CClientItem * pWhoseItem);
	void SetRevBoxText(CString strMsg);
	void RemoveClientFromArray(CClientItem in_item);

public://ui button
	CStatic m_L1;
	CStatic m_L2;
	CStatic m_L3;
	CStatic m_L4;
	HICON m_hIcon_gray;
	HICON m_hIcon_green;
	HICON m_hIcon_red;
	HICON m_hIcon_orange;

protected:
	CPngButton m_print;

public:
	afx_msg void OnBnClickedBtnPrint();
	//==bg
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);//bg和另一个也用
	//CBitmap m_bmp;   //位图
	//CBrush m_brush;  //画刷

public://用于让窗口和背景相适应
	int m_oldWidth;   //窗口的宽
	int m_oldHeight;  //窗口的高
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
