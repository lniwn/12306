// TBHeaderCtrl.cpp
//

#include "stdafx.h"
#include "TBListCtrl.h"
#include "TBHeaderCtrl.h"


// CHeaderCtrlCl

IMPLEMENT_DYNAMIC(CTBHeaderCtrl, CHeaderCtrl)

CTBHeaderCtrl::CTBHeaderCtrl()
: m_R(207)
, m_G(207)
, m_B(207)
, m_Gradient(2)
{
	m_Height = 1;
	m_fontHeight = 15;
	m_fontWith = 0;
	m_color = RGB(0,0,0);
	m_pListCtrl = NULL;
}

CTBHeaderCtrl::~CTBHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CTBHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
END_MESSAGE_MAP()


void CTBHeaderCtrl::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting
	DrawHeader(&dcPaint);
}


LRESULT CTBHeaderCtrl::OnLayout( WPARAM wParam, LPARAM lParam )
{
	UNUSED_ALWAYS(wParam);
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam); 
	HD_LAYOUT &hdl = *( HD_LAYOUT * ) lParam; 
	RECT *prc = hdl.prc; 
	WINDOWPOS *pwpos = hdl.pwpos; 

	
	int nHeight = (int)(pwpos->cy * m_Height);
	pwpos->cy = nHeight; 
	prc->top = nHeight; 
	return lResult; 
}




BOOL CTBHeaderCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	NMHDR& nmh = *(NMHDR*)lParam;
	if (HDN_BEGINTRACK == nmh.code)
	{
		// 禁止第一列拖动
		/*LPNMHEADER phdn = (LPNMHEADER)lParam;
		if (0 == phdn->iItem)
		{
			*pLResult = TRUE;
			return TRUE;
		}*/
	}
	else if (HDN_ITEMCHANGING == nmh.code
		|| HDN_ITEMCHANGED == nmh.code)
	{
		LPNMHEADER phdr = (LPNMHEADER)lParam;
		if (phdr->pitem->cxy < 10)
		{
			phdr->pitem->cxy = 11;
			*pLResult = TRUE;
		}
		((CTBListCtrl*)m_pListCtrl)->UpdateComponentPos();
	}
	else if (HDN_DIVIDERDBLCLICK == nmh.code)
	{
		*pLResult = TRUE;
		return TRUE;
	}
	
	return CHeaderCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}


void CTBHeaderCtrl::SetListCtrl(CListCtrl* pListCtrl)
{
	m_pListCtrl = pListCtrl;
}


void CTBHeaderCtrl::DrawHeader(CDC* pDC)
{
	bool bGet = false;
	if (NULL == pDC)
	{
		pDC = GetDC();
		bGet = true;
	} 
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap memBmp;
	CRect memRect;
	GetClientRect(&memRect);
	memBmp.CreateCompatibleBitmap(pDC, memRect.Width(), memRect.Height());
	CBitmap* pOldBmp = dc.SelectObject(&memBmp);
	dc.FillSolidRect(0, 0, memRect.Width(), memRect.Height(), RGB(m_R, m_G, m_B));

	int nItem; 
	nItem = GetItemCount();
	for(int i = 0; i<nItem;i ++) 
	{ 
		CRect tRect;
		GetItemRect(i,&tRect);
		int R = m_R,G = m_G,B = m_B;
		CRect nRect(tRect);
		nRect.left++;

		for(int j = tRect.top;j<=tRect.bottom;j++) 
		{ 
			nRect.bottom = nRect.top+1; 
			CBrush _brush; 
			_brush.CreateSolidBrush(RGB(R,G,B));
			dc.FillRect(&nRect,&_brush); 
			_brush.DeleteObject(); 
			R-=m_Gradient;G-=m_Gradient;B-=m_Gradient;
			if (R<0)R = 0;
			if (G<0)G = 0;
			if (B<0)B= 0;
			nRect.top = nRect.bottom; 
		} 
		dc.SetBkMode(TRANSPARENT);
		CFont nFont ,* nOldFont; 
		//dc.SetTextColor(RGB(250,50,50)); 
		dc.SetTextColor(m_color);
		HDITEM hdItem;
		ZeroMemory(&hdItem, sizeof(hdItem));
		hdItem.mask = HDI_TEXT;
		hdItem.pszText = new TCHAR[256];
		ZeroMemory(hdItem.pszText, 256);
		hdItem.cchTextMax = 256;
		if (GetItem(i, &hdItem))
		{
			nFont.CreateFont(m_fontHeight,m_fontWith,0,0,0,FALSE,FALSE,0,0,0,0,0,0,_TEXT("SimSun"));
			nOldFont = dc.SelectObject(&nFont);

			TEXTMETRIC metric;
			dc.GetTextMetrics(&metric);
			int ofst = 0;
			ofst = tRect.Height() - metric.tmHeight;
			tRect.OffsetRect(0,ofst/2);
			dc.DrawText(hdItem.pszText,&tRect,DT_CENTER);

			dc.SelectObject(nOldFont); 
			nFont.DeleteObject(); 
		}
		delete []hdItem.pszText;
	} 

	CRect rtRect;
	CRect clientRect;
	GetItemRect(nItem - 1,rtRect);
	GetClientRect(clientRect);
	rtRect.left = rtRect.right+1;
	rtRect.right = clientRect.right;
	int R = m_R,G = m_G,B = m_B;
	CRect nRect(rtRect);

	for(int j = rtRect.top;j<=rtRect.bottom;j++) 
	{ 
		nRect.bottom = nRect.top+1; 
		CBrush _brush; 
		_brush.CreateSolidBrush(RGB(R,G,B));
		dc.FillRect(&nRect,&_brush);
		_brush.DeleteObject(); 
		R-=m_Gradient;G-=m_Gradient;B-=m_Gradient;
		if (R<0)R = 0;
		if (G<0)G = 0;
		if (B<0)B= 0;
		nRect.top = nRect.bottom; 
	} 

	pDC->BitBlt(0, 0, memRect.Width(), memRect.Height(), &dc, 0, 0, SRCCOPY);
	dc.SelectObject(pOldBmp);
	memBmp.DeleteObject();
	dc.DeleteDC();

	if (bGet)
	{
		ReleaseDC(pDC);
	}
}