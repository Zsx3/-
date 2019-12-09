#include "stdafx.h"
#include "transparent_for_light.h"

IMPLEMENT_DYNAMIC(CTransparentPic, CStatic)

CTransparentPic::CTransparentPic()
	: m_nBitmapIndex(0)
{
}

CTransparentPic::~CTransparentPic()
{
}

BEGIN_MESSAGE_MAP(CTransparentPic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CTransparentPic::SetBitmapIndex(UINT nBitmapIndex)
{
	ModifyStyle(SS_BITMAP, 0);  // Remove bitmap style, use owner paint

	bool bForceRedraw = (m_nBitmapIndex != nBitmapIndex);

	m_nBitmapIndex = nBitmapIndex;

	// Force owner paint
	if (::IsWindow(m_hWnd) && bForceRedraw)
	{
		Invalidate();
	}
}

// CTransparentPic message handlers
BOOL CTransparentPic::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CTransparentPic::OnPaint()
{
	CWnd::Default(); // Calls the default window procedure

	CClientDC dc(this);
	CBitmap bmp;

	// Try to load bitmap
	if (bmp.LoadBitmap(m_nBitmapIndex))
	{
		CDC memDC;

		memDC.CreateCompatibleDC(NULL);

		CBitmap* pOldBmp = memDC.SelectObject(&bmp);
		BITMAP bitmap;

		bmp.GetBitmap(&bitmap);
		TransparentBlt(dc.m_hDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight,
			memDC.m_hDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(236, 233, 216));

		memDC.SelectObject(pOldBmp);
		memDC.DeleteDC();
	}
	else
	{
		// Draw error information
		dc.TextOut(1, 1, _T("ERROR!"));
	}
}