
// all_mfc_uiDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "all_mfc_ui.h"
#include "all_mfc_uiDlg.h"
#include "afxdialogex.h"
#include "auto.h"
#include "CvvImage.h"
#include <iostream>
#include "user_msg.h"
#include "afxwin.h"
#include "aca.h"
#include "transparent_for_light.h"
using namespace std;
#define WINDOW_NAME "opencv_wnd"
#define POINT_MAX_NUM 18

Mat img_1,frame,clear_pic;
Point start = Point(-1, -1);
Point mouse = Point(-1, -1); //划线用
Point end_point = Point(-1, -1);
Rect_s rect_result; //图像识别
Rect roi;
Point* point_array;
//vector<point> point_array;
int rect;
int num_rect;//框体区域标号
bool rect_exist = 0;
bool down_flag = false, up_flag = true;
bool ok_flag = false;
bool paint_rect_btn_flag, paint_point_btn_flag;
bool exit_flag=false;
 //创建了长度为count的动态数组

int count_ponit = 0;

HANDLE Paint_point_hThread;			//创建独立线程发送数据
HANDLE Paint_rect_hThread;			//创建独立线程发送数据

CEvent sure_btn_event;  //只能用于线程？？用在普通函数（启动按钮）的while中无效果
bool sure_flag = false;//用于非线程函数的标识

CEvent datadownload_event;
CEvent clear_event;
CEvent close_event;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框


#define MAX_BUFF 256
char szRev[MAX_BUFF] = { 0 };
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CallmfcuiDlg 对话框



CallmfcuiDlg::CallmfcuiDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALL_MFC_UI_DIALOG, pParent)
	, m_ServicePort(8260)
	, m_current_name(_T(""))
	, m_OutputString(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CallmfcuiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_paint_rect_btn, m_paint_rect);
	DDX_Control(pDX, IDC_paint_point_btn, m_paint_point);
	DDX_Control(pDX, IDC_clear_btn, m_clear);
	DDX_Control(pDX, IDC_sure_btn, m_sure);
	DDX_Control(pDX, IDC_data_down_load_btn, m_datadownload);
	DDX_Control(pDX, IDC_read_pic, m_start);
	DDX_Text(pDX, IDC_EDITPORT, m_ServicePort);
	DDX_Control(pDX, IDC_EDIT2, m_EditRevBox);
	DDX_Control(pDX, IDC_BUTTONSTART, m_server_start);
	DDX_Control(pDX, IDC_BUTTONEND, m_server_end);
	DDX_Control(pDX, IDC_STATIC_L1, m_L1);
	DDX_Control(pDX, IDC_STATIC_L2, m_L2);
	DDX_Control(pDX, IDC_STATIC_L3, m_L3);
	DDX_Control(pDX, IDC_STATIC_L4, m_L4);
	DDX_Text(pDX, IDC_name, m_current_name);
	DDX_Text(pDX, IDC_OutputString, m_OutputString);
}

BEGIN_MESSAGE_MAP(CallmfcuiDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_paint_rect_btn, &CallmfcuiDlg::OnBnClickedpaintrectbtn)
	ON_BN_CLICKED(IDC_paint_point_btn, &CallmfcuiDlg::OnBnClickedpaintpointbtn)
	ON_BN_CLICKED(IDC_clear_btn, &CallmfcuiDlg::OnBnClickedclearbtn)
	ON_BN_CLICKED(IDC_sure_btn, &CallmfcuiDlg::OnBnClickedsurebtn)
	ON_BN_CLICKED(IDC_data_down_load_btn, &CallmfcuiDlg::OnBnClickeddatadownloadbtn)
	ON_BN_CLICKED(IDC_exit_btn, &CallmfcuiDlg::OnBnClickedexitbtn)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_read_pic, &CallmfcuiDlg::OnBnClickedreadpic)
	ON_BN_CLICKED(IDC_BUTTONSTART, &CallmfcuiDlg::OnBnClickedButtonstart)
	ON_BN_CLICKED(IDC_BUTTONSEND, &CallmfcuiDlg::OnBnClickedButtonsend)

	ON_EN_CHANGE(IDC_EDITSENDBOX, &CallmfcuiDlg::OnEnChangeEditsendbox)
//	ON_BN_CLICKED(IDC_BUTTON_D_1, &CallmfcuiDlg::OnBnClickedButtonD1)
	ON_BN_CLICKED(IDC_BUTTONEND, &CallmfcuiDlg::OnBnClickedButtonend)
	ON_BN_CLICKED(IDC_BTN_PRINT, &CallmfcuiDlg::OnBnClickedBtnPrint)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_OutputString, &CallmfcuiDlg::OnEnChangeOutputstring)
END_MESSAGE_MAP()


// CallmfcuiDlg 消息处理程序

BOOL CallmfcuiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标




	// TODO: 在此添加额外的初始化代码=================================================
	namedWindow(WINDOW_NAME, WINDOW_AUTOSIZE);
	HWND hwndShow=(HWND)cvGetWindowHandle(WINDOW_NAME);//获得opencv窗口的句柄
	//父窗口
	HWND hParent = ::GetParent(hwndShow);//原父窗口
	::SetParent(hwndShow, GetDlgItem(IDC_STATIC)->m_hWnd);//把opencv窗口的句柄作为picture控件的子窗口句柄
	//函数原型 HWND SetParent(HWND hWndChild,HWND hWndNewParent)

	//隐藏原来的父窗口
	::ShowWindow(hParent, SW_HIDE);
	//CRect pic_rect;
	//GetDlgItem(IDC_STATIC)->GetWindowRect(&pic_rect);
	GetDlgItem(IDC_STATIC)->MoveWindow(120, 120, 582, 438, 1);

	//按钮状态初始化
	m_paint_rect.EnableWindow(FALSE);
	m_paint_point.EnableWindow(FALSE);
	m_clear.EnableWindow(FALSE);
	m_sure.EnableWindow(FALSE);
	m_datadownload.EnableWindow(FALSE);
	m_start.EnableWindow(FALSE);//需等待开始信号
	//m_start.EnableWindow(TRUE);//调试用
	::EnableWindow(GetDlgItem(IDC_BUTTONSEND)->m_hWnd, FALSE);
	::EnableWindow(GetDlgItem(IDC_BUTTONEND)->m_hWnd, FALSE);
	SetDlgItemText(IDC_EDITPORT, _T("8263"));
	GetDlgItem(IDC_EDITPORT)->SetFocus();


	//==============================UI初始化==================
	m_hIcon_gray = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(ICON_GRAY), IMAGE_ICON, 40, 40, LR_DEFAULTCOLOR);
	m_hIcon_red = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(ICON_RED), IMAGE_ICON, 40, 40, LR_DEFAULTCOLOR);
	m_hIcon_green = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(ICON_GREEN), IMAGE_ICON, 40, 40, LR_DEFAULTCOLOR);
	m_hIcon_orange = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(ICON_ORANGE), IMAGE_ICON, 40, 40, LR_DEFAULTCOLOR);
	m_L1.SetIcon(m_hIcon_orange);
	m_L2.SetIcon(m_hIcon_gray);
	m_L3.SetIcon(m_hIcon_gray);
	m_L4.SetIcon(m_hIcon_gray);

	m_print.SubclassDlgItem(IDC_BTN_PRINT, this);

	m_print.SetImagePng(IDB_PNG_PRINT);

	m_print.SetColor(RGB(255, 255, 255), 100, 155);//設置按鈕顯示半透明貼膜
	//==背景与窗口适应 没用
	CRect rect;
	GetClientRect(&rect);
	m_oldWidth = rect.Width();    //将初始的宽和高保存起来，当窗口大小改变的时候用得上
	m_oldHeight = rect.Height();

	//===显示操作人员名称及编号
	m_OutputString = _T("编号");
	m_current_name = current_user_name;
	UpdateData(false);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CallmfcuiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CallmfcuiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);

		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;		
		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();//否则无法显示背景
		HBITMAP   hBitmap1;
		hBitmap1 = LoadBitmap(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_BITMAP1)); // IDB_BITMAP_TEST为资源图片ID
		((CButton *)GetDlgItem(IDC_paint_rect_btn))->SetBitmap(hBitmap1);


		HBITMAP   hBitmap2;
		hBitmap2 = LoadBitmap(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_BITMAP2)); // IDB_BITMAP_TEST为资源图片ID
		((CButton *)GetDlgItem(IDC_paint_point_btn))->SetBitmap(hBitmap2);

		HBITMAP   hBitmap3;
		hBitmap3 = LoadBitmap(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_BITMAP3)); // IDB_BITMAP_TEST为资源图片ID
		((CButton *)GetDlgItem(IDC_clear_btn))->SetBitmap(hBitmap3);

		HBITMAP   hBitmap4;
		hBitmap4 = LoadBitmap(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_BITMAP4)); // IDB_BITMAP_TEST为资源图片ID
		((CButton *)GetDlgItem(IDC_sure_btn))->SetBitmap(hBitmap4);

		HBITMAP   hBitmap5;
		hBitmap5 = LoadBitmap(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_BITMAP5)); // IDB_BITMAP_TEST为资源图片ID
		((CButton *)GetDlgItem(IDC_data_down_load_btn))->SetBitmap(hBitmap5);

		HBITMAP   hBitmap6;
		hBitmap6 = LoadBitmap(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_BITMAP6)); // IDB_BITMAP_TEST为资源图片ID
		((CButton *)GetDlgItem(IDC_exit_btn))->SetBitmap(hBitmap6);


		CRect rect;
		CPaintDC dc(this);
		GetClientRect(&rect);
		dc.FillSolidRect(rect, RGB(141, 182, 205));
		dc.FillPath();
		GetClientRect(&rect);
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap bmpBackground;
		bmpBackground.LoadBitmap(IDB_BITMAP_BG);   //IDB_BITMAP_BACKGROUND是你自己的图对应的ID 
		BITMAP bitmap;
		bmpBackground.GetBitmap(&bitmap);
		CBitmap *pbmpOld = dcMem.SelectObject(&bmpBackground);
		dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CallmfcuiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//=======================================================画框模式==============================
bool rect_inout(Rect rect, Point point)
{
	if (
		point.x <= rect.br().x && point.x >= rect.tl().x &&
		point.y <= rect.br().y && point.y >= rect.tl().y
		)
		return 1;
	else return 0;
}
void on_mouse_rect(int event, int x, int y, int flags, void* ustc)
{

	Mat& image = *(cv::Mat*)ustc;//这样就可以传递Mat信息了，很机智
	if (paint_rect_btn_flag == TRUE)
	{
		switch (event)
		{
		case CV_EVENT_LBUTTONDOWN://按下左键
		{

			start = Point(x, y);
			mouse = Point(x, y);
			down_flag = true;
			if (rect_exist == 0) // 画框
			{

				rect = 1;

			}
			if (rect_exist == 1)
			{
				int area = 20;//矩形判断区域宽度
				//矩形区域标定 四个角为正方形 其余上下左右四个部分矩形框 相互连接不覆盖 从tl起始为1 顺时针排布
				//用向量重构 减少代码体积 
				Rect top_mid = cv::Rect(Point(rect_result.tl_x + area, rect_result.tl_y - area), Point(rect_result.br_x - area, rect_result.tl_y + area)), // 描述矩形框附近的8个拖动判断区域
					top_left = cv::Rect(Point(rect_result.tl_x - area, rect_result.tl_y - area), Point(rect_result.tl_x + area, rect_result.tl_y + area)),
					top_right = cv::Rect(Point(rect_result.br_x - area, rect_result.tl_y - area), Point(rect_result.br_x + area, rect_result.tl_y + area)),

					mid_left = cv::Rect(Point(rect_result.tl_x - area, rect_result.tl_y + area), Point(rect_result.tl_x + area, rect_result.br_y - area)),
					mid_right = cv::Rect(Point(rect_result.br_x - area, rect_result.tl_y + area), Point(rect_result.br_x + area, rect_result.br_y - area)),

					bottom_mid = cv::Rect(Point(rect_result.tl_x + area, rect_result.br_y - area), Point(rect_result.br_x - area, rect_result.br_y + area)),
					bottom_left = cv::Rect(Point(rect_result.tl_x - area, rect_result.br_y - area), Point(rect_result.tl_x + area, rect_result.br_y + area)),
					bottom_right = cv::Rect(Point(rect_result.br_x - area, rect_result.br_y - area), Point(rect_result.br_x + area, rect_result.br_y + area));

				if (rect_inout(top_mid, Point(x, y))) num_rect = 2; //判断是否处于目标区域 
				else
					if (rect_inout(top_left, Point(x, y))) num_rect = 1;
					else
						if (rect_inout(top_right, Point(x, y))) num_rect = 3;
						else
							if (rect_inout(mid_left, Point(x, y))) num_rect = 8;
							else
								if (rect_inout(mid_right, Point(x, y))) num_rect = 4;
								else
									if (rect_inout(bottom_mid, Point(x, y))) num_rect = 6;
									else
										if (rect_inout(bottom_left, Point(x, y))) num_rect = 7;
										else
											if (rect_inout(bottom_right, Point(x, y))) num_rect = 5;
											else num_rect = 0;



			}
		}break;
		case CV_EVENT_MOUSEMOVE:
		{
			mouse = Point(x, y);
			if (rect_exist == 0 && mouse != start && down_flag == true)
			{
				Mat move_temp = image.clone();////////////////////////////////////////强退bug(通过设定paint_rect_btn_flag退出onMouse解决)
				rectangle(move_temp, start, mouse, Scalar(0, 0, 200), 2);
				move_temp.copyTo(frame);
				//imshow(WINDOW_NAME, move_temp);


			}
			else if (rect_exist == 1 && down_flag == true && mouse != start)//
			{
				switch (num_rect)
				{
					//根据矩形区域 赋予对应的坐标
				case 0:
					break;
				case 1:
					start = Point(x, y);
					rect_result.tl_x = start.x;
					rect_result.tl_y = start.y;
					break;
				case 2:
					start = Point(rect_result.tl_x, y);
					rect_result.tl_y = start.y;
					break;
				case 3:
					start = Point(rect_result.tl_x, y);
					rect_result.tl_y = start.y;
					end_point = Point(x, rect_result.br_y);
					rect_result.br_x = end_point.x;
					break;
				case 4:
					end_point = Point(x, rect_result.br_y);
					rect_result.br_x = end_point.x;
					break;
				case 5:
					end_point = Point(x, y);
					rect_result.br_x = end_point.x;
					rect_result.br_y = end_point.y;
					break;
				case 6:
					end_point = Point(rect_result.br_x, y);
					rect_result.br_y = end_point.y;
					break;
				case 7:
					start = Point(x, rect_result.tl_y);
					rect_result.tl_x = start.x;
					end_point = Point(rect_result.br_x, y);
					rect_result.br_y = end_point.y;
					break;
				case 8:
					start = Point(x, rect_result.tl_y);
					rect_result.tl_x = start.x;
					break;
				}


				Mat move1_temp = image.clone();
				rectangle(move1_temp, Point(rect_result.tl_x, rect_result.tl_y), Point(rect_result.br_x, rect_result.br_y), cv::Scalar(0, 0, 200), 2, 2);
				move1_temp.copyTo(frame);
				//imshow(WINDOW_NAME, move1_temp);

			}
		}break;

		case EVENT_LBUTTONUP:
		{
			down_flag = false;
			end_point = Point(x, y);
			if (rect_exist == 0)
			{
				rect_exist = 1;

				Mat up_temp = image.clone();
				rectangle(up_temp, start, end_point, Scalar(0, 0, 200), 2);
				rect_result.tl_x = start.x;
				rect_result.tl_y = start.y;
				rect_result.br_x = end_point.x;
				rect_result.br_y = end_point.y;
				up_temp.copyTo(frame);

			}
			else if (rect_exist == 1 && down_flag == false)
			{
				Mat up1_temp = image.clone();
				rectangle(up1_temp, Point(rect_result.tl_x, rect_result.tl_y), Point(rect_result.br_x, rect_result.br_y), cv::Scalar(0, 0, 200), 2, 2);
				up1_temp.copyTo(frame);
				//imshow(WINDOW_NAME, up1_temp);
			}
		}break;
		case EVENT_RBUTTONDOWN:
		{
			//ok_flag = true;
			//cvui::init(WINDOW_NAME);

			//Mat ok_temp = image.clone();
			//rectangle(ok_temp, Point(rect_result.tl_x, rect_result.tl_y), Point(rect_result.br_x, rect_result.br_y), cv::Scalar(0, 200, 0), 2, 2);
			//ok_temp.copyTo(frame);
			//imshow(WINDOW_NAME, ok_temp);
			//exit_flag = TRUE;
		}
		}
	}
}

DWORD WINAPI Paint_Rect_Thread(LPVOID lpParam)
{
	CallmfcuiDlg* pThis = (CallmfcuiDlg*)lpParam;
	CRect rect;//用于获取控件矩形
	//获取控件picture控件ID
	CWnd *pWnd = pThis->GetDlgItem(IDC_STATIC);
	//获取picture控件的大小
	pWnd->GetClientRect(&rect);
	//=======================================图片失真需优化=======================//这是是需要调整图片与空间的尺寸关系，是否要放在线程调整，待优化
	//int x = rect.Width();
	//int y = rect.Height();
	//resize(img_1, img_1, Size(x, y));
	//============================================================================
	//先清屏
	frame = clear_pic.clone();

	Mat temp = frame.clone();//用于本btn  temp 临时图片

	while (waitKey(1) != 27)
	{
		
		if (WaitForSingleObject(sure_btn_event, 5) == WAIT_OBJECT_0)
		{
			_endthreadex(0);
			break;

		}
		else
		{
			setMouseCallback(WINDOW_NAME, on_mouse_rect, (void*)& temp);
			imshow(WINDOW_NAME, frame);

			//============================刷新导致闪屏=======================
			//temp = clear_pic.clone();//防止矩形残留
			//rectangle(temp, Point(rect_result.tl_x, rect_result.tl_y), Point(rect_result.br_x, rect_result.br_y), cv::Scalar(0, 0, 200), 2, 2);
			//temp.copyTo(frame);
			//===============================================================
		}
		if (rect_exist == 1)
		{
			pThis->m_sure.EnableWindow(TRUE);
		}

	}
	//imshow(WINDOW_NAME, temp);
	return 0;
}

void CallmfcuiDlg::OnBnClickedpaintrectbtn()
{
	//清屏
	frame = clear_pic.clone();
	if (paint_rect_btn_flag == TRUE)
	{
		TerminateThread(Paint_rect_hThread, 0);
	}
	
	//===rect====
	//初始化
	rect_exist = 0;
	rect_result = { -1,-1,-1,-1 };




	paint_rect_btn_flag = TRUE;
	m_paint_point.EnableWindow(FALSE);
	// TODO: 在此添加控件通知处理程序代码
	DWORD ThreadSendID_r;
	Paint_rect_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Paint_Rect_Thread, (LPVOID)this, 0, &ThreadSendID_r);
	WaitForSingleObject(Paint_Rect_Thread, INFINITE);//直到按下确定，break弹出循环，该线程才正式结束

	//CloseHandle(Paint_rect_hThread);

}

//=======================================================取点模式==============================
void on_mouse_point(int event, int x, int y, int flags, void* ustc)
{
	


	if (paint_point_btn_flag == TRUE)
	{
		if (event == EVENT_LBUTTONDOWN&&count_ponit <POINT_MAX_NUM)
		{
			circle(frame, Point(x, y), 4, Scalar(0, 0, 200), 2);
			point_array[count_ponit] = Point(x, y);
			count_ponit++;
			//imshow(WINDOW_NAME, frame);
		}
	}

}
DWORD WINAPI Paint_Point_Thread(LPVOID lpParam)
{
	CallmfcuiDlg* pThis = (CallmfcuiDlg*)lpParam;
	CRect rect;
	//获取控件picture控件ID
	CWnd *pWnd = pThis->GetDlgItem(IDC_STATIC);
	//获取picture控件的大小
	pWnd->GetClientRect(&rect);
	//=======================================图片失真需优化=======================
	//int x = rect.Width();
	//int y = rect.Height();
	//resize(img_1, img_1, Size(x, y));
	//============================================================================//这是是需要调整图片与空间的尺寸关系，是否要放在线程调整，待优化
	//先清屏
	frame = clear_pic.clone();

	Mat temp = frame.clone();//用于本btn  temp 临时图片

	while (TRUE)
	{

		if (WaitForSingleObject(sure_btn_event, 5) == WAIT_OBJECT_0)
		{
			_endthreadex(0);
			break;
		}
		else
		{
			setMouseCallback(WINDOW_NAME, on_mouse_point, (void*)& pThis);
			//imshow(WINDOW_NAME, frame);
			if (count_ponit == POINT_MAX_NUM)
			{
				pThis->m_sure.EnableWindow(TRUE);
			}
		}


	}
	//imshow(WINDOW_NAME, temp);
	return 0;
}
void CallmfcuiDlg::OnBnClickedpaintpointbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxMessageBox(_T("使用取点模式并按下确定键后会将您的信息记录在后台"));

	frame = clear_pic.clone();
	if (paint_point_btn_flag == TRUE)
	{
		TerminateThread(Paint_point_hThread, 0);
	}
	
	//===point====
	//初始化
	if (count_ponit > 0)
	{
		count_ponit = 0;
	}


	point_array= new Point[POINT_MAX_NUM];//创建了长度为count的动态数组
	count_ponit = 0;

	m_paint_rect.EnableWindow(FALSE);
	paint_point_btn_flag = TRUE;
	DWORD ThreadSendID_p;
	Paint_point_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Paint_Point_Thread, (LPVOID)this, 0, &ThreadSendID_p);
	WaitForSingleObject(Paint_Rect_Thread, INFINITE);//直到按下确定，break弹出循环，该线程才正式结束


}

//=======================================================重新画框==============================
void CallmfcuiDlg::OnBnClickedclearbtn()
{
	// TODO: 在此添加控件通知处理程序代码

	//===================UI============ 
	m_L1.SetIcon(m_hIcon_gray);
	m_L2.SetIcon(m_hIcon_green);
	m_L3.SetIcon(m_hIcon_orange);
	m_L4.SetIcon(m_hIcon_gray);


	//清屏
	frame = clear_pic.clone();
	TerminateThread(Paint_rect_hThread, 0);
	TerminateThread(Paint_point_hThread, 0);



	//===rect====
	//初始化
	delete[] point_array;
	point_array = new Point[POINT_MAX_NUM];
	if (rect_exist == 1)
	{
		rect_exist = 0;
		rect_result = { -1,-1,-1,-1 };
		paint_rect_btn_flag = FALSE;
	}



	//===point====
	//初始化
	if (count_ponit>0)
	{
		count_ponit = 0;
		point_array=0;
		paint_point_btn_flag = FALSE;	
	}

	
	m_paint_rect.EnableWindow(TRUE);
	m_paint_point.EnableWindow(TRUE);
	m_sure.EnableWindow(FALSE);
	m_datadownload.EnableWindow(FALSE);
}

//=======================================================确定=================================
Point* Get_Point(double tl_x, double tl_y, double width, double height, int n, int n_x, int n_y)
//参数从左到右分别：左上角x 左上角y 宽 高 随机点数 矩形x方向等分数 矩形y方向等分数
{
	Point* temp_array = new Point[n];
	int* mark = new int[n];
	srand((int)(time(NULL)));
	int* r = new int[n];
	int temp, tempn = 0;
	//得到n个小于n_x*n_y的随机数储存在r数组中
	while (tempn != n)
	{

		int flag = 0;
		temp = rand() % (n_x*n_y);
		for (int i = 0; i < tempn; i++)
		{
			if (r[i] == temp)
			{
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			continue;
		r[tempn] = temp;
		tempn++;
	}
	int row, column;
	int xx, yy;
	for (int i = 0; i < n; i++)
	{

		row = r[i] / n_x;
		column = r[i] % n_x;
		int dx = (rand() % 3) * (width / n_x / 3) + (width / n_x / 6);
		int dy = (rand() % 3) * (height / n_y / 3) + (height / n_y / 6);
		xx = int(tl_x + (width / n_x) * column + dx);
		yy = int(tl_y + (height / n_y) * row + dy);

		if ((column % 2) == 0)
		{
			mark[i] = (column + 1) * n_y - row;
		}
		else mark[i] = column * 4 + 1 + row;//得到随机点的序号
		int flag = 0;
		for (int m = 0; m < i; m++)
		{
			if (mark[m] == mark[i]) flag = 1;

		}
		if (flag == 1)
		{
			i--;
			continue;
		}
		temp_array[i] = Point(xx, yy);

	}
	return temp_array;
}
int* Any_Colony_Algorithm(double C[POINT_MAX_NUM][2])
{
	time_t timer, timerl;

	time(&timer);
	unsigned long seed = timer;
	seed %= 56000;
	srand((unsigned int)seed);

	//由矩阵表示两两城市之间的距离
	calculateAllDistance();
	//蚁群系统对象
	AntColonySystem* acs = new AntColonySystem();
	ACSAnt* ants[M];
	//蚂蚁均匀分布在城市上
	for (int k = 0; k < M; k++)
	{
		ants[k] = new ACSAnt(acs, (int)(k%N));
	}
	calculateAllDistance();
	//随机选择一个节点计算由最近邻方法得到的一个长度
	int node = rand() % N;
	Lnn = CalAdjacentDistance(node);

	//各条路径上初始化的信息素强度
	double initInfo = 1 / (N * Lnn);
	acs->InitParameter(initInfo);

	//全局最优路径
	int globalTour[N][2];
	//全局最优长度
	double globalBestLength = 0.0;
	for (int i = 0; i < NcMax; i++)
	{
		//局部最优路径
		int localTour[N][2];
		//局部最优长度
		double localBestLength = 0.0;
		//当前路径长度
		double tourLength;
		for (int j = 0; j < M; j++)
		{
			int* tourPath = ants[j]->Search();
			tourLength = calculateSumOfDistance(tourPath);
			//局部比较，并记录路径和长度
			if (tourLength < localBestLength || abs(localBestLength - 0.0) < 0.000001)
			{
				for (int m = 0; m < N; m++)
				{
					int row = *(tourPath + 2 * m);
					int col = *(tourPath + 2 * m + 1);
					localTour[m][0] = row;
					localTour[m][1] = col;
				}
				localBestLength = tourLength;
			}
		}
		//全局比较，并记录路径和长度
		if (localBestLength < globalBestLength || abs(globalBestLength - 0.0) < 0.000001)
		{
			for (int m = 0; m < N; m++)
			{
				globalTour[m][0] = localTour[m][0];
				globalTour[m][1] = localTour[m][1];
			}
			globalBestLength = localBestLength;
		}
		acs->UpdateGlobalPathRule(*globalTour, globalBestLength);
		int*res = new int[N];
		for (int m = 0; m < N; m++)
		{
			res[m] = globalTour[m][0];
		}
		cout << endl;
		time(&timerl);
		int t = timerl - timer;
		return res;
	}
}

void CallmfcuiDlg::OnBnClickedsurebtn()
{
	// TODO: 在此添加控件通知处理程序代码	
	//===================UI============ 
	m_L1.SetIcon(m_hIcon_gray);
	m_L2.SetIcon(m_hIcon_gray);
	m_L3.SetIcon(m_hIcon_orange);
	m_L4.SetIcon(m_hIcon_gray);



	sure_btn_event.SetEvent();
	sure_flag = TRUE;

	frame = clear_pic.clone();//先清屏
	m_paint_rect.EnableWindow(FALSE);
	m_paint_point.EnableWindow(FALSE);
	//===rect====
	paint_rect_btn_flag = FALSE;

	if (rect_exist == 1)
	{
		Mat ok_temp;
		ok_temp = clear_pic.clone();
		rectangle(ok_temp, Point(rect_result.tl_x, rect_result.tl_y), Point(rect_result.br_x, rect_result.br_y), cv::Scalar(50, 0, 0), 2, 2);
		point_array = new Point[POINT_MAX_NUM];
		point_array = Get_Point(rect_result.tl_x, rect_result.tl_y, rect_result.get_width(), rect_result.get_height(), POINT_MAX_NUM, 6, 4);
		
		for (int i = 0; i < POINT_MAX_NUM; i++)
		{
			circle(ok_temp, point_array[i], 4, Scalar(0, 200, 0), 2);
		}
		ok_temp.copyTo(frame);
	}

	////===point====
	if (count_ponit == POINT_MAX_NUM)
	{
		for (int i = 0; i < POINT_MAX_NUM; i++)
		{
			circle(frame, point_array[i], 4, Scalar(0, 200, 0), 2);
		}

		//===记录图片及管理人信息==
			/*
			格式： 操作人+操作时间
			*/
		CString msg;
		msg = _T("的本次操作已被记录在后台");
		AfxMessageBox(current_user_name+msg);

		CTime temp_time;
		CString current_time_and_name;
		temp_time = CTime::GetCurrentTime();             //获取当前时间日期
		CString chinese_expreesion = temp_time.Format("%w");//星期转换为中文的表达方式

		int p = atoi(chinese_expreesion);
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
		current_time_and_name =  current_user_name+ " " +temp_time.Format(_T("%Y-%m-%d %H-%M-%S")) + " " + CString(week)+".png";
		cv::String pic_name= LPCTSTR(current_time_and_name);
		imwrite(pic_name, frame);

		//imwrite(current_time_and_name.GetBuffer(),frame);
	}
	m_datadownload.EnableWindow(TRUE);
	m_sure.EnableWindow(FALSE);

	//Point->double[][2]
	for (int i = 0; i < POINT_MAX_NUM; i++)
	{
		temp_double_array[i][0] = point_array[i].x;
		temp_double_array[i][1] = point_array[i].y;
	}
	for (int i = 0; i < POINT_MAX_NUM; i++)
	{
		circle(frame, Point(temp_double_array[i][0], temp_double_array[i][1]), 2, Scalar(0, 0, 200), 2);
	}
	// 当前temp_double_array的序列是根据point_array确定的
	//point_array 框选是随机生成的，数据点的序号未知，但每个坐标点的位置都知道，乱序
	//点选是手动生成，乱序
	//按照生成的顺序作为这18个点的序号
	//到时候根据蚁群算法生成的序号读取temp_double_array中的坐标点，按照给的排序，来将
	//temp_double_array重新排序，生成一个新的sorted_point_array,依次连接sorted_point_array
	//坐标点即可得到最短路径,但起点也是随机，因为是闭环,所以到时候将最左上角的点作为起点
	//（假设采样机构的起始位置在左上角｝
	int* shortest_sort;
	shortest_sort = Any_Colony_Algorithm(temp_double_array);//这里是根据temp_double_array输入进去的顺序来作为0-17排序，这个shortest_sort也是以当前temp输出的一个排序
	//还需找出最大点作为起点
	int min_index = -1;
	int min_sum =1e6;
	for (int j = 0; j < POINT_MAX_NUM; j++)
	{
		if ((sqrt(pow(temp_double_array[shortest_sort[j]][0],2) + pow(temp_double_array[shortest_sort[j]][1],2))) < min_sum)
		{
			min_index = j;//这个索引是原temp的，和shortest的第min_index没关系
			min_sum = sqrt(pow(temp_double_array[shortest_sort[j]][0], 2) + pow(temp_double_array[shortest_sort[j]][1], 2));
		}
	}
	//putText(frame, cv::format("%d", min_index), Point(temp_double_array[min_index][0], temp_double_array[min_index][1]), 1, 1, Scalar(0, 0, 255));

	int *new_sort=new int[POINT_MAX_NUM];//以最小点为起点重新排序
	for (int k = 0; k < POINT_MAX_NUM; k++)
	{
		new_sort[k] = shortest_sort[min_index + k];
		if ((min_index + k) > (POINT_MAX_NUM - 1))
		{
			new_sort[k]= shortest_sort[min_index + k- POINT_MAX_NUM];
		}
	}
	double sorted_point_array[POINT_MAX_NUM][2];
	for (int q = 0; q < POINT_MAX_NUM; q++)
	{
		sorted_point_array[q][0] = temp_double_array[new_sort[q]][0];
		sorted_point_array[q][1] = temp_double_array[new_sort[q]][1];
	}
	for (int p = 0; p < POINT_MAX_NUM - 1; p++)
	{
		line(frame, Point(sorted_point_array[p][0], sorted_point_array[p][1]), Point(sorted_point_array[p + 1][0], sorted_point_array[p + 1][1]), Scalar(0, 255, 0), 1, 1);
		putText(frame, cv::format("%d",p+1), Point(sorted_point_array[p][0], sorted_point_array[p][1]), 1,1,Scalar(0, 0, 255));
	}
	putText(frame, cv::format("%d", POINT_MAX_NUM ), Point(sorted_point_array[POINT_MAX_NUM - 1][0], sorted_point_array[POINT_MAX_NUM - 1][1]), 1, 1, Scalar(0, 0, 255));

}


//=======================================================数据下载==============================
void CallmfcuiDlg::OnBnClickeddatadownloadbtn()
{
	// TODO: 在此添加控件通知处理程序代码	
	//===================UI============ 
	m_L1.SetIcon(m_hIcon_gray);
	m_L2.SetIcon(m_hIcon_gray);
	m_L3.SetIcon(m_hIcon_green);
	m_L4.SetIcon(m_hIcon_gray);

	//AfxMessageBox(_T("success"));
	m_paint_rect.EnableWindow(FALSE);
	m_paint_point.EnableWindow(FALSE);
	m_clear.EnableWindow(FALSE);
	m_sure.EnableWindow(FALSE);
	m_datadownload.EnableWindow(FALSE);
	m_start.EnableWindow(FALSE);//需等待开始信号 调试注释
	//发送当前的数据点



	//初始化当前的数据点及读取最新的图片
	start = Point(-1, -1);
	mouse = Point(-1, -1); 
	end_point = Point(-1, -1);
	rect_exist = 0;
	down_flag = false;
	up_flag = true;
	ok_flag = false;
	paint_rect_btn_flag = false;
	paint_point_btn_flag = false;
	count_ponit = 0;
	rect_result = {};
	point_array = 0;
	frame = imread("car0.png");
	imshow(WINDOW_NAME, frame);




}

//=======================================================X&&exit退出==============================
void CallmfcuiDlg::OnBnClickedexitbtn()
{
	close_event.SetEvent();
	exit_flag = true;
	CTime temp_time;
	temp_time = CTime::GetCurrentTime();             //获取当前时间日期
	CString chinese_expreesion = temp_time.Format("%w");//星期转换为中文的表达方式

	int p = atoi(chinese_expreesion);
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
	user_information logout_ifm;
	logout_ifm.m_time_logout = temp_time.Format(_T("%Y-%m-%d %H:%M:%S")) + " " + CString(week);
	MessageBox(TEXT(logout_ifm.m_time_logout));
	user_msg file;
	file.WriteDocline1(logout_ifm.m_time_logout, 3);


	CDialogEx::OnClose();
	CloseHandle(Paint_point_hThread);
	CloseHandle(Paint_rect_hThread);
	cv::destroyAllWindows();
	delete[] point_array;
	exit(0);

	//断开连接
	m_ClientArray.RemoveAll();
	closesocket(m_SockListen);
}


void CallmfcuiDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	close_event.SetEvent();
	exit_flag = true;
	CTime temp_time;
	temp_time = CTime::GetCurrentTime();             //获取当前时间日期
	CString chinese_expreesion = temp_time.Format("%w");//星期转换为中文的表达方式

	int p = atoi(chinese_expreesion);
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
	user_information logout_ifm;
	logout_ifm.m_time_logout = temp_time.Format(_T("%Y-%m-%d %H:%M:%S")) + " " + CString(week);
	MessageBox(TEXT(logout_ifm.m_time_logout));
	user_msg file;
	file.WriteDocline1(logout_ifm.m_time_logout, 3);


	CDialogEx::OnClose();
	CloseHandle(Paint_point_hThread);
	CloseHandle(Paint_rect_hThread);
	cv::destroyAllWindows();
	delete[] point_array;
	exit(0);
	
	//断开连接
	m_ClientArray.RemoveAll();
	closesocket(m_SockListen);
}

//=======================================================启动==============================
void CallmfcuiDlg::OnBnClickedreadpic()
{
	
	// TODO: 在此添加控件通知处理程序代码
	//===================UI============ 
	m_L1.SetIcon(m_hIcon_gray);
	m_L2.SetIcon(m_hIcon_green);
	m_L3.SetIcon(m_hIcon_orange);
	m_L4.SetIcon(m_hIcon_gray);

	m_start.EnableWindow(FALSE); 

	Mat img_1 = imread("car1.png");
	if (img_1.data)
	{
		m_paint_rect.EnableWindow(TRUE);
		m_paint_point.EnableWindow(TRUE);
		m_clear.EnableWindow(TRUE);
		//m_sure.EnableWindow(TRUE);
		//m_datadownload.EnableWindow(FALSE);
	}
	else
	{
		AfxMessageBox(TEXT("图片读取失败"));

		m_L4.SetIcon(m_hIcon_red);
	}
	frame = img_1.clone();
	clear_pic = img_1.clone();
	while (waitKey(10) != 27)
	{
		imshow(WINDOW_NAME, frame);		

		if (exit_flag==true)
		{
			break;
		}	

	}


}


//===================================================================SOCKET===================================================================
BOOL CallmfcuiDlg::StartServer(void)
{
	m_hListenThread = CreateThread(NULL, 0, ListenThreadFunc, this, 0, 0);
	if (m_hListenThread == NULL)
	{
		return FALSE;
	}
	else {
		return TRUE;
	}
}
DWORD WINAPI ListenThreadFunc(LPVOID Lparam)
{
	CallmfcuiDlg * pServer = (CallmfcuiDlg *)Lparam;
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	if (INVALID_SOCKET == (pServer->m_SockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
	{
		AfxMessageBox(_T("建立socket失败"));
		return 0;
	}
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(int(pServer->m_ServicePort));
	if (SOCKET_ERROR == ::bind(pServer->m_SockListen, (sockaddr *)&service, sizeof(sockaddr_in)))
	{
		AfxMessageBox(_T("绑定端口失败"));
		return 0;
	}
	if (0 != listen(pServer->m_SockListen, 5))
	{
		AfxMessageBox(_T("监听端口失败"));
		return 0;
	}

	//提示建立socket成功
	pServer->m_server_start.EnableWindow(FALSE);
	//	pServer->EnableWindow(IDC_BUTTONSEND,TRUE);
	pServer->m_server_end.EnableWindow(TRUE);
	pServer->SetRevBoxText(_T("服务启动成功,开始监听端口"));
	//进入循环，监听端口
	while (TRUE)
	{
		if (socket_Select(pServer->m_SockListen, 100, TRUE))
		{
			sockaddr_in clientAddr;
			int iLen = sizeof(sockaddr_in);
			SOCKET accSock = accept(pServer->m_SockListen, (struct sockaddr *)&clientAddr, &iLen);
			if (accSock == INVALID_SOCKET)
			{
				continue;
			}
			//将节点加入链中
			CClientItem tItem;
			tItem.m_ClientSocket = accSock;
			tItem.m_strIp = inet_ntoa(clientAddr.sin_addr); //IP地址
			tItem.m_pMainWnd = pServer;
			int idx = pServer->m_ClientArray.Add(tItem); ///idx是第x个连接的客户端
			tItem.m_hThread = CreateThread(NULL, 0, ClientThreadProc, //创建一个线程并挂起:CREATE_SUSPENDED
				&(pServer->m_ClientArray.GetAt(idx)), CREATE_SUSPENDED, NULL);
			pServer->m_ClientArray.GetAt(idx).m_hThread = tItem.m_hThread;
			//等把hThread加入了节点，才开始执行线程，如下
			ResumeThread(tItem.m_hThread);
			pServer->SetRevBoxText(tItem.m_strIp + _T("上线"));
			Sleep(100);
		}
		//==============================================在监听线程里处理接受的信号来对启动键置位
		char open_msg[256] = {"1111"};
		char close_msg[256] = { "0000" };
		if (strcmp(szRev, open_msg)==0)
		{
			pServer->m_start.EnableWindow(TRUE);
			//pServer->MessageBox(TEXT("该打开"));
			memset(szRev, ' ', sizeof(szRev));
		}
		if (strcmp(szRev, close_msg) == 0)
		{
			pServer->m_start.EnableWindow(FALSE);
			memset(szRev, ' ', sizeof(szRev));
		}

	}
}

void CallmfcuiDlg::SendClientMsg(CString strMsg, CClientItem * pWhoseItem)
{
	USES_CONVERSION;
	char szBuf[256] = { 0 };
	//strcpy_s(szBuf, 256, T2A(strMsg));
	strcpy_s(szBuf, 256, (LPSTR)(LPCTSTR)strMsg);
	
	for (int i = 0; i < m_ClientArray.GetCount(); i++)
	{
		if (!pWhoseItem || !equal(pWhoseItem, &(m_ClientArray.GetAt(i))))
		{
			send(m_ClientArray.GetAt(i).m_ClientSocket, szBuf, 256, 0);
		}
	}
}
BOOL CallmfcuiDlg::equal(const CClientItem * p1, const CClientItem * p2)
{
	if (p1->m_ClientSocket == p2->m_ClientSocket && p1->m_hThread == p2->m_hThread && p1->m_strIp == p2->m_strIp)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//#define MAX_BUFF 256
DWORD WINAPI ClientThreadProc(LPVOID Lparam) { //利用异步IO模型循环读取socket内的信息，并发送给各个用户  客户端处理线程
	USES_CONVERSION;
	CString strMsg;
	CClientItem ClientItem = *(CClientItem *)Lparam;
	while (TRUE)
	{
		if (socket_Select(ClientItem.m_ClientSocket, 100, TRUE))
		{
			//char szRev[MAX_BUFF] = { 0 };
			int iRet = recv(ClientItem.m_ClientSocket, szRev, sizeof(szRev), 0);
			//CString str;
			//str.Format(_T("%d"), iRet);
			//AfxMessageBox(str);
			if (iRet > 0)
			{
				strMsg = A2T(szRev); //中文出现乱码，英文正常
//				strMsg.Format(_T("%s"),szRev); //这么写连英文都不对了
				ClientItem.m_pMainWnd->SetRevBoxText(ClientItem.m_strIp + _T(">>") + strMsg);
				ClientItem.m_pMainWnd->SendClientMsg(strMsg, &ClientItem);
			}
			else {
				strMsg = ClientItem.m_strIp + _T(" 已离开");
				ClientItem.m_pMainWnd->RemoveClientFromArray(ClientItem);
				ClientItem.m_pMainWnd->SetRevBoxText(strMsg);
				break;
			}
		}
	}
	return 0;
}
void CallmfcuiDlg::RemoveClientFromArray(CClientItem in_item) 
{
	for (int idx = 0; idx < m_ClientArray.GetCount(); idx++)
	{
		if (in_item.m_ClientSocket == m_ClientArray[idx].m_ClientSocket &&
			in_item.m_hThread == m_ClientArray[idx].m_hThread &&
			in_item.m_strIp == m_ClientArray[idx].m_strIp &&
			in_item.m_pMainWnd == m_ClientArray[idx].m_pMainWnd)
		{
			m_ClientArray.RemoveAt(idx);
		}
	}
	return;
}
CString GetTime() 
{
	SYSTEMTIME time;
	CString strTime;
	GetLocalTime(&time);
	strTime.Format(_T("%d%02d%02d %02d:%02d"), time.wYear, time.wMonth, time.wDay,time.wHour, time.wMinute);
	return strTime;
}
void CallmfcuiDlg::SetRevBoxText(CString strMsg) {
	m_EditRevBox.SetSel(-1, -1);
	m_EditRevBox.ReplaceSel(GetTime() + _T("\r\n  ") + strMsg + _T("\r\n"));
}

void CallmfcuiDlg::OnEnChangeEditsendbox()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strMsg;
	GetDlgItemText(IDC_EDITSENDBOX, strMsg);
	if (strMsg == _T("") || !m_isServerOpen)
	{
		::EnableWindow(GetDlgItem(IDC_BUTTONSEND)->m_hWnd, FALSE);
	}
	else 
	{
		::EnableWindow(GetDlgItem(IDC_BUTTONSEND)->m_hWnd, TRUE);
	}
}

void CallmfcuiDlg::OnBnClickedButtonstart()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ServicePort = GetDlgItemInt(IDC_EDITPORT);
	if (m_ServicePort <= 1024 || m_ServicePort > 65535)
	{
		AfxMessageBox(_T("请输入合适端口"));
		SetDlgItemText(IDC_EDITPORT, _T(""));
		GetDlgItem(IDC_EDITPORT)->SetFocus();
		return;
	}
	StartServer();
	m_isServerOpen = TRUE;
	OnEnChangeEditsendbox(); //如果发送框中原有内容，则使发送按钮有效
}

BOOL socket_Select(SOCKET hSocket, DWORD nTimeOut, BOOL bRead) {
	FD_SET fdset;
	timeval tv;
	FD_ZERO(&fdset);
	FD_SET(hSocket, &fdset);
	nTimeOut = nTimeOut > 1000 ? 1000 : nTimeOut;
	tv.tv_sec = 0;
	tv.tv_usec = nTimeOut;
	int iRet = 0;
	if (bRead)
	{
		iRet = select(0, &fdset, NULL, NULL, &tv);
	}
	else
	{
		iRet = select(0, NULL, &fdset, NULL, &tv);
	}
	if (iRet <= 0)
	{
		return FALSE;
	}
	else if (FD_ISSET(hSocket, &fdset))
	{
		return TRUE;
	}
	return FALSE;
}

void CallmfcuiDlg::OnBnClickedButtonsend()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strMsg;
	GetDlgItemText(IDC_EDITSENDBOX, strMsg);
	SendClientMsg(strMsg, NULL);
	SetRevBoxText(_T("服务器>>") + strMsg);
	SetDlgItemText(IDC_EDITSENDBOX, _T(""));
}



//void CallmfcuiDlg::OnBnClickedButtonD1()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	LedFlag = !LedFlag;
//	if (LedFlag)
//	{
//		m_button_1.LoadBitmaps(IDB_BITMAP_RED); 
//		m_button_1.Invalidate();
//	}
//	else
//	{
//		m_button_1.LoadBitmaps(IDB_BITMAP_GRAY);
//		m_button_1.Invalidate();
//	}
//}


void CallmfcuiDlg::OnBnClickedButtonend()
{
	// TODO: 在此添加控件通知处理程序代码
	int AllClient = m_ClientArray.GetCount();
	for (int idx = 0; idx < AllClient; idx++)
	{
		closesocket(m_ClientArray.GetAt(idx).m_ClientSocket);
	}
	m_ClientArray.RemoveAll();
	closesocket(m_SockListen);
	
	::EnableWindow(GetDlgItem(IDC_BUTTONSEND)->m_hWnd, FALSE);
	::EnableWindow(GetDlgItem(IDC_BUTTONSTART)->m_hWnd, TRUE);
	::EnableWindow(GetDlgItem(IDC_BUTTONEND)->m_hWnd, FALSE);
	SetRevBoxText(_T("停止监听端口"));
	m_isServerOpen = FALSE;
}


void CallmfcuiDlg::OnBnClickedBtnPrint()
{
	// TODO: 在此添加控件通知处理程序代码
}


HBRUSH CallmfcuiDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
		//==bg==
	//m_bmp.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP8));    //这里将位图加载进资源后，再Load
	//m_brush.CreatePatternBrush(&m_bmp);
	//if (nCtlColor == CTLCOLOR_DLG)
	//{
	//	return m_brush;
	//}
	//==static 透明

	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	//将界面上static控件的背景设为透明
	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC_1:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	case IDC_STATIC_2:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	case IDC_STATIC_3:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	case IDC_STATIC_4:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	case IDC_STATIC_5:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	default:
		break;

		return hbr;
	}

	//==使灯透明，（191，191，191)为背景设置的颜色
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		static HBRUSH hbrStatic = ::CreateSolidBrush(RGB(191, 191, 191));
		//COLOR是你想设置的背景颜色 此处必须为静态变量，否则不能实现
		pDC->SetBkColor(RGB(191, 191, 191));
		return hbrStatic; //返回该刷
	}
	return hbr;
}


void CallmfcuiDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	Invalidate(FALSE);
}



void CallmfcuiDlg::OnEnChangeOutputstring()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
