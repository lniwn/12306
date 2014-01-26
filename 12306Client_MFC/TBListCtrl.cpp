// TBListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TBListCtrl.h"
#include <algorithm>
#include <typeinfo>

#include "TBColorButton.h"
// CTBListCtrl
HHOOK CTBListCtrl::m_hHook = NULL;
CWnd* CTBListCtrl::m_pWnd;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CTBListCtrl, CListCtrl)

CTBListCtrl::CTBListCtrl()
	:m_dwRowHeight(0x20), m_clrLineColor(RGB(0xC0,0xFF,0x3E)),
	m_clrSelectedRowColor(RGB(0x87,0xce,0xeb)),
	m_dwFontWidth(0),
	m_dwFontHeight(12)
{
	m_pWnd = this;
	m_dwExStyle = 0;
}

CTBListCtrl::~CTBListCtrl()
{
}


BEGIN_MESSAGE_MAP(CTBListCtrl, CListCtrl)
	//ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//ON_WM_HSCROLL()
	//ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT_EX(LVN_ENDSCROLL, &CTBListCtrl::OnLvnEndScroll)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, &CTBListCtrl::OnNMClick)
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CTBListCtrl::OnNMCustomDraw)
	ON_MESSAGE(BN_CLICKED, &CTBListCtrl::OnBnClickedCallBack)
END_MESSAGE_MAP()



// CTBListCtrl message handlers


void CTBListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->hDC);
	DrawNormalItem(lpDrawItemStruct);
	CRect rect;
	GetClientRect(&rect);
	CRect lastRect;
	GetItemRect(GetItemCount()-1, &lastRect, LVIR_BOUNDS);

	if (lastRect.bottom < rect.bottom)
	{
		rect.top = lastRect.bottom + 1;
		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CBrush tmpBrush;
		tmpBrush.CreateSolidBrush(DEFAULT_NORMAL_BK_COLOR);
		pDC->FillRect(&rect, &tmpBrush);
		tmpBrush.DeleteObject();
	}
}


BOOL CTBListCtrl::IsItemVisible(int iIndex)
{
	const int iTopIndex = GetTopIndex();
	const int iBottomIndex = iTopIndex + GetCountPerPage();
	return !(iIndex < iTopIndex || iIndex > iBottomIndex);
}


void CTBListCtrl::DrawNormalItem(LPDRAWITEMSTRUCT lpDrawItem)
{
	ASSERT(ODT_LISTVIEW == lpDrawItem->CtlType);
	if(!IsItemVisible(lpDrawItem->itemID)) return;
	ItemMap::iterator it = m_mapItemInfo.find(lpDrawItem->itemID);
	ASSERT(it != m_mapItemInfo.end());
	PITEM_INFO pstItem = it->second;
	CDC *pWholeDC = new CDC;
	pWholeDC->Attach(lpDrawItem->hDC);
	CRect clientRect;
	GetClientRect(&clientRect);

	CDC memDC;
	CBitmap memBitmap;
	VERIFY(memDC.CreateCompatibleDC(pWholeDC));
	memBitmap.CreateCompatibleBitmap(pWholeDC, clientRect.Width(), clientRect.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&memBitmap);
	memDC.SetBkMode(TRANSPARENT);

	RECT rectDraw = {0};
	const CRect rectDrawItem = lpDrawItem->rcItem;

	lpDrawItem->rcItem.left = 0;
	rectDraw = lpDrawItem->rcItem;
	rectDraw.right = clientRect.right;

	CBrush tmpBrush;
	tmpBrush.CreateSolidBrush(pstItem->clrBkColor);
	memDC.FillRect(&rectDraw, &tmpBrush);
	if (lpDrawItem->itemState & ODS_SELECTED)
	{
		//tmpBrush.CreateSolidBrush(m_clrSelectedRowColor);
		//memDC.DrawEdge(&rectDraw, EDGE_RAISED, BF_FLAT);
		memDC.DrawFocusRect(&rectDraw);
	} 
	else
	{
		//tmpBrush.CreateSolidBrush(pstItem->clrBkColor);
		/*memDC.FillRect(&rectDraw, &tmpBrush);
		tmpBrush.DeleteObject();*/
	}
	
	tmpBrush.DeleteObject();
	
	if (GetExtendedStyle() & LVS_EX_CHECKBOXES)
	{
		CRect subRect;
		GetSubItemRect(lpDrawItem->itemID, 0, LVIR_ICON, subRect);
		
		subRect.left = rectDrawItem.left + 3;
		subRect.right = subRect.left + 12;
		
		if (pstItem->bSelected)
		{
			memDC.DrawFrameControl(&subRect, DFC_BUTTON, DFCS_BUTTONCHECK|DFCS_CHECKED|DFCS_ADJUSTRECT|DFCS_FLAT);
		}
		else
		{
			memDC.DrawFrameControl(&subRect, DFC_BUTTON, DFCS_BUTTONCHECK|DFCS_ADJUSTRECT|DFCS_MONO);
		}
	}

	RECT bottomLine(rectDraw);
	bottomLine.top = rectDraw.bottom - 1;
	tmpBrush.CreateSolidBrush(m_clrLineColor);
	memDC.FillRect(&bottomLine, &tmpBrush);
	tmpBrush.DeleteObject();

	int iCount = GetHeaderCtrl()->GetItemCount();
	CRect subRect;
	for (int i = 0; i < iCount; ++i)
	{
		if (0 == i)
		{
			GetFirstItemRect(lpDrawItem->itemID, &subRect);
		} 
		else
		{
			GetSubItemRect(lpDrawItem->itemID, i, LVIR_LABEL, subRect);
		}
		CString strTxt(GetItemText(lpDrawItem->itemID, i));
		if (!strTxt.IsEmpty())
		{
			memDC.SetTextColor(GetItemTextColor(lpDrawItem->itemID, i));
			CFont* pFont = GetFont();
			LOGFONT lf;
			pFont->GetLogFont(&lf);
			CFont newFont;
			LOGFONT newLf = lf;
			newLf.lfWidth = m_dwFontWidth;
			newLf.lfHeight = m_dwFontHeight;
			newFont.CreateFontIndirect(&newLf);
			HFONT hOldFont = (HFONT)memDC.SelectObject(newFont);
			subRect.OffsetRect(2,0);
			memDC.DrawText(strTxt, &subRect, DT_SINGLELINE | DT_VCENTER
				| DT_LEFT | DT_NOCLIP | DT_END_ELLIPSIS);
			memDC.SelectObject(hOldFont);
			newFont.DeleteObject();
		}
	}
	
	rectDraw.left = lpDrawItem->rcItem.left;
	rectDraw.right = clientRect.right;
	
	pWholeDC->BitBlt(rectDraw.left, rectDraw.top, clientRect.Width(),
		rectDraw.bottom - rectDraw.top, &memDC, rectDraw.left, rectDraw.top, SRCCOPY);
	memDC.SelectObject(pOldBmp);
	memBitmap.DeleteObject();
	memDC.DeleteDC();
	pWholeDC->Detach();
	delete pWholeDC;
}


void CTBListCtrl::GetFirstItemRect(int iItem, LPRECT pRect)
{
	ZeroMemory(pRect, sizeof(RECT));

	int iCount = GetHeaderCtrl()->GetItemCount();
	if (iCount <= 1) return;

	if (!GetItemRect(iItem, pRect, LVIR_BOUNDS)) return;

	CRect tmpRect;
	if (!GetSubItemRect(iItem, 0, LVIR_LABEL, tmpRect)) return;
	pRect->right = tmpRect.right;
	pRect->left += 2;
}


COLORREF CTBListCtrl::GetItemTextColor(int iItem, int iSubItem)
{
	map<int, PITEM_INFO>::iterator it = m_mapItemInfo.find(iItem);
	if (m_mapItemInfo.end() == it)
	{
		return DEFAULT_NORMAL_TEXT_COLOR;
	}
	else
	{
		list<SUBITEM_INFO>::iterator itSub = it->second->listSubItem.begin();
		list<SUBITEM_INFO>::iterator itSubEnd = it->second->listSubItem.end();
		for (; itSub != itSubEnd; ++itSub)
		{
			if (itSub->iSubItem == iSubItem)
			{
				return itSub->clrTextColor;
			}
		}
	}
	return DEFAULT_NORMAL_TEXT_COLOR;
}


COLORREF CTBListCtrl::GetItemBkColor(int iItem)
{
	
	map<int, PITEM_INFO>::iterator it = m_mapItemInfo.find(iItem);
	if (m_mapItemInfo.end() == it)
	{
		return DEFAULT_NORMAL_BK_COLOR;
	}
	return it->second->clrBkColor;
}


int CTBListCtrl::InsertItem(int iItem, LPCTSTR lpszItem, COLORREF clrBkColor, COLORREF clrTextColor /* = DEFAULT_NORMAL_TEXT_COLOR */)
{
	if (iItem > GetItemCount())
	{
		return -1;
	}
	
	PITEM_INFO pItemInfo = new ITEM_INFO;
	pItemInfo->clrBkColor = clrBkColor;
	pItemInfo->clrNormalColor = clrTextColor;
	pItemInfo->bSelected = false;
	SUBITEM_INFO subItem;
	subItem.iSubItem = 0;
	subItem.clrTextColor = clrTextColor;
	subItem.clrBkColor = clrBkColor;
	subItem.pBtn = NULL;
	subItem.strNormal = lpszItem;
	pItemInfo->listSubItem.push_back(subItem);

	ItemMap::iterator it = m_mapItemInfo.find(iItem);
	ItemMap::iterator itEnd = m_mapItemInfo.end();
	if(it == itEnd)
	{
		m_mapItemInfo.insert(std::make_pair(iItem, pItemInfo));
	}
	else
	{
		ItemMap tmp;
		for (it = m_mapItemInfo.begin(); it != itEnd;)
		{
			if (it->first >= iItem)
			{
				PITEM_INFO pItem = it->second;
				int iIndex = it->first+1;
				m_mapItemInfo.erase(it++);
				tmp.insert(std::make_pair(iIndex, pItem));
			}
			else
			{
				++it;
			}
		}
		std::for_each(tmp.begin(), tmp.end(), [&](ItemMap::reference ref)
		{
			m_mapItemInfo.insert(ref);
		});
		m_mapItemInfo.insert(std::make_pair(iItem, pItemInfo));
	}
	
	return CListCtrl::InsertItem(iItem, lpszItem);
}


int CTBListCtrl::InsertItem(int iItem, PITEM_INFO pItemInfo)
{
	if (iItem > GetItemCount())
	{
		return -1;
	}
	ItemMap::iterator it = m_mapItemInfo.find(iItem);
	ItemMap::iterator itEnd = m_mapItemInfo.end();
	if(it == itEnd)
	{
		m_mapItemInfo.insert(std::make_pair(iItem, pItemInfo));
	}
	else
	{
		ItemMap tmp;
		for (it = m_mapItemInfo.begin(); it != itEnd;)
		{
			if (it->first >= iItem)
			{
				PITEM_INFO pItem = it->second;
				int iIndex = it->first+1;
				m_mapItemInfo.erase(it++);
				tmp.insert(std::make_pair(iIndex, pItem));
			}
			else
			{
				++it;
			}
		}
		std::for_each(tmp.begin(), tmp.end(), [&](ItemMap::reference ref)
		{
			m_mapItemInfo.insert(ref);
		});
		m_mapItemInfo.insert(std::make_pair(iItem, pItemInfo));
	}
	return CListCtrl::InsertItem(iItem, pItemInfo->listSubItem.front().strNormal);
}


BOOL CTBListCtrl::SetItemBkColor(int iItem, COLORREF clrBkColor)
{
	ItemMap::iterator it = m_mapItemInfo.find(iItem);
	if (m_mapItemInfo.end() == it)
	{
		PITEM_INFO itemInfo = new ITEM_INFO;
		itemInfo->clrBkColor = clrBkColor;
		//itemInfo->dwFlags = LISTITEM_NORMAL;
		m_mapItemInfo.insert(std::make_pair(iItem, itemInfo));
		return FALSE;
	} 
	else
	{
		it->second->clrBkColor = clrBkColor;
		return TRUE;
	}
}


void CTBListCtrl::SetRowHeight(int iItemHeight/* = 0x20*/)
{
	m_dwRowHeight = static_cast<DWORD>(iItemHeight);

	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, static_cast<WPARAM>(0), reinterpret_cast<LPARAM>(&wp));
}

//void CTBListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
//{
//	lpMeasureItemStruct->itemHeight = m_dwRowHeight;
//
//	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
//}

void CTBListCtrl::MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	if (m_dwRowHeight > 0)
	{
		lpMeasureItemStruct->itemHeight = m_dwRowHeight;
	}
}


BOOL CTBListCtrl::SetItemTextColor(int iItem, int iSubItem, COLORREF clrColor)
{
	map<int, PITEM_INFO>::iterator it = m_mapItemInfo.find(iItem);
	if (m_mapItemInfo.end() == it)
	{
		PITEM_INFO pItemInfo = new ITEM_INFO;
		pItemInfo->clrBkColor = DEFAULT_NORMAL_BK_COLOR;
		pItemInfo->clrNormalColor = DEFAULT_NORMAL_TEXT_COLOR;

		SUBITEM_INFO subItemInfo;
		subItemInfo.clrTextColor = clrColor;
		subItemInfo.iSubItem = iSubItem;
		subItemInfo.pBtn = NULL;
		pItemInfo->listSubItem.push_back(subItemInfo);
		m_mapItemInfo.insert(std::make_pair(iItem, pItemInfo));
		return FALSE;
	} 
	else
	{
		list<SUBITEM_INFO>::iterator subIt = it->second->listSubItem.begin();
		list<SUBITEM_INFO>::iterator subItEnd = it->second->listSubItem.end();
		for (; subIt != subItEnd; ++subIt)
		{
			if (iSubItem == subIt->iSubItem)
			{
				subIt->clrTextColor = clrColor;
				return TRUE;
			}
		}
		SUBITEM_INFO subItemInfo;
		subItemInfo.clrTextColor = clrColor;
		subItemInfo.iSubItem = iSubItem;
		subItemInfo.pBtn = NULL;
		it->second->listSubItem.push_back(subItemInfo);
		return TRUE;
	}
}


BOOL CTBListCtrl::SetItemText(_In_ int nItem, _In_ int nSubItem, _In_z_ LPCTSTR lpszText, COLORREF clrTextColor /* = DEFAULT_NORMAL_TEXT_COLOR */)
{
	if (nSubItem >= GetHeaderCtrl()->GetItemCount())
	{
		return FALSE;
	}

	ItemMap::iterator itItem = m_mapItemInfo.find(nItem);
	if (m_mapItemInfo.end() != itItem)
	{
		list<SUBITEM_INFO>::iterator itSub, itSubEnd;
		itSub = itItem->second->listSubItem.begin();
		itSubEnd = itItem->second->listSubItem.end();

		for (; itSub != itSubEnd; ++itSub)
		{
			if (itSub->iSubItem == nSubItem)
			{
				itSub->strNormal = lpszText;
				if (DEFAULT_NORMAL_TEXT_COLOR != clrTextColor)
				{
					SetItemTextColor(nItem, nSubItem, clrTextColor);
				}
				return CListCtrl::SetItemText(nItem, nSubItem, lpszText);
			}
		}

		SUBITEM_INFO subItemInfo;
		subItemInfo.clrBkColor = DEFAULT_NORMAL_BK_COLOR;
		subItemInfo.clrTextColor = clrTextColor;
		subItemInfo.iSubItem = nSubItem;
		subItemInfo.pBtn = NULL;
		subItemInfo.strNormal = lpszText;
		itItem->second->listSubItem.push_back(subItemInfo);
		return CListCtrl::SetItemText(nItem, nSubItem, lpszText);
	}
	else
	{
		return FALSE;
	}
}


void CTBListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();

	ClearMapInfo();
	if (m_hHook)
	{
		VERIFY(::UnhookWindowsHookEx(m_hHook));
	}
}


void CTBListCtrl::ClearMapInfo()
{
	std::for_each(m_mapItemInfo.begin(), m_mapItemInfo.end(),
		[](ItemMap::reference item)
	{
		std::for_each(item.second->listSubItem.begin(), item.second->listSubItem.end(),
			[](list<SUBITEM_INFO>::reference subItem)
		{
			if (subItem.pBtn)
			{
				subItem.pBtn->DestroyWindow();
				delete subItem.pBtn;
			}
		});
		delete item.second;
	});
	m_mapItemInfo.clear();
}


BOOL CTBListCtrl::DeleteItem(_In_ int nItem)
{
	if (nItem >= GetItemCount())
	{
		return FALSE;
	}
	ItemMap::iterator it = m_mapItemInfo.find(nItem);
	ItemMap::iterator itEnd = m_mapItemInfo.end();
	if(it != itEnd)
	{
		delete it->second;
		m_mapItemInfo.erase(it);
	}

	ItemMap tmp;
	for (it = m_mapItemInfo.begin(); it != itEnd;)
	{
		if (it->first > nItem)
		{
			PITEM_INFO pItem = it->second;
			int iIndex = it->first-1;
			m_mapItemInfo.erase(it++);
			tmp.insert(std::make_pair(iIndex, pItem));
		}
		else
		{
			++it;
		}
	}
	std::for_each(tmp.begin(), tmp.end(), [&](ItemMap::reference ref)
	{
		m_mapItemInfo.insert(ref);
	});
	//DeleteSelectedItem(nItem);
	UpdateComponentPos();
	return CListCtrl::DeleteItem(nItem);
}


BOOL CTBListCtrl::DeleteColumn(_In_ int nCol)
{
	int iCount = GetHeaderCtrl()->GetItemCount();
	if (nCol >= iCount || 1 == iCount)
	{
		return FALSE;
	}
	list<SUBITEM_INFO>::iterator itSub, itSubDel, itSubEnd;
	ItemMap::iterator itEnd = m_mapItemInfo.end();
	for (ItemMap::iterator it = m_mapItemInfo.begin(); it != itEnd; ++it)
	{
		itSub = it->second->listSubItem.begin();
		itSubEnd = it->second->listSubItem.end();
		itSubDel = itSubEnd;
		for (; itSub != itSubEnd; ++itSub)
		{
			if (itSub->iSubItem == nCol)
			{
				itSubDel = itSub;
			} 
			else if (itSub->iSubItem > nCol)
			{
				--itSub->iSubItem;
			}
		}
		if (itSubDel != itSubEnd)
		{
			if (itSubDel->pBtn)
			{
				itSubDel->pBtn->DestroyWindow();
				delete itSubDel->pBtn;
			}
			it->second->listSubItem.erase(itSubDel);
		}
	}
	UpdateComponentPos();
	return CListCtrl::DeleteColumn(nCol);
}


BOOL CTBListCtrl::DeleteAllItems()
{
	ClearMapInfo();
	m_vecSelectedItem.clear();
	return CListCtrl::DeleteAllItems();
}


void CTBListCtrl::InvalidAndUpdateWindow()
{
	if (::IsWindow(m_hWnd))
	{
		RECT rc;
		GetClientRect(&rc);
		InvalidateRect(&rc);
		UpdateWindow();
		m_headerCtrl.Invalidate(FALSE);
	}
}


void CTBListCtrl::InvalidAndUpdateItem(int iItem)
{
	if (::IsWindow(m_hWnd) && iItem < GetItemCount())
	{
		RECT rect;
		if (CListCtrl::GetItemRect(iItem, &rect, LVIR_BOUNDS))
		{
			InvalidateRect(&rect);
			UpdateWindow();
		}
	}
}


void CTBListCtrl::SetLineColor(COLORREF clrLineColor)
{
	m_clrLineColor = clrLineColor;
}


void CTBListCtrl::SetSelectedRowColor(COLORREF clrRow)
{
	m_clrSelectedRowColor = clrRow;
}

BOOL CTBListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//CListCtrl::OnEraseBkgnd(pDC);
	CRect rectErase;
	GetUpdateRect(&rectErase);
	CBrush brush(DEFAULT_NORMAL_BK_COLOR);
	pDC->FillRect(&rectErase, &brush);
	//m_headerCtrl.DrawHeader(pDC);
	return TRUE;
	//return CListCtrl::OnEraseBkgnd(pDC);
}


BOOL CTBListCtrl::InitListCtrl()
{
	/*m_hHook = ::SetWindowsHookEx(WH_CBT, CBTProc, NULL,
	::GetWindowThreadProcessId(GetParent()->GetSafeHwnd(), NULL));
	ASSERT(m_hHook);
	return NULL != m_hHook;*/
	LONG lStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	lStyle |= (LVS_REPORT|WS_CLIPCHILDREN|LVS_OWNERDRAWFIXED);
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, lStyle);

	LONG lExStyle = GetExtendedStyle();
	lExStyle |= (LVS_EX_DOUBLEBUFFER|LVS_EX_TRANSPARENTBKGND);
	lExStyle &= (~LVS_EX_GRIDLINES);
	SetExtendedStyle(lExStyle);
	
	m_headerCtrl.SetListCtrl(this);
	return TRUE;
}


LRESULT CALLBACK CTBListCtrl::CBTProc(int code,WPARAM wParam,LPARAM lParam)
{
	if (HCBT_MOVESIZE == code)
	{
		if (::IsWindowEnabled(m_pWnd->GetSafeHwnd()))
		{
			CRect rcClient, rcParentClient;
			m_pWnd->GetParent()->GetClientRect(&rcParentClient);
			/*m_pWnd->GetWindowRect(&rcClient);
			m_pWnd->GetParent()->ScreenToClient(&rcClient);*/
			rcClient.top = 11;
			rcClient.left = 44;
			rcClient.right = rcParentClient.right - 48;
			rcClient.bottom = rcParentClient.bottom - 50;
			m_pWnd->MoveWindow(rcClient);
			
		}
		
	}
	else if (HCBT_MINMAX == code)
	{
		WORD wFlag = LOWORD(lParam);
		if (SW_MAXIMIZE == wFlag)
		{
			CRect rcParentClient, rcClient;
			VERIFY(::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcParentClient, 0));
			rcClient.top = 11;
			rcClient.left = 44;
			rcClient.right = rcParentClient.right - 48;
			rcClient.bottom = rcParentClient.bottom - 50;
			m_pWnd->MoveWindow(rcClient);
		}
	}
	return ::CallNextHookEx(m_hHook, code, wParam, lParam);
}


void CTBListCtrl::SetFontWidthHeight(DWORD dwWidth, DWORD dwHeight)
{
	m_dwFontWidth = dwWidth;
	m_dwFontHeight = dwHeight;
}

void CTBListCtrl::SetHeaderHeightRatio(float fRatio)
{
	m_headerCtrl.m_Height = fRatio;
}


void CTBListCtrl::SetHeaderBkColor(int R, int G, int B, int iGradient)
{
	m_headerCtrl.m_R = R;
	m_headerCtrl.m_G = G;
	m_headerCtrl.m_B = B;
	m_headerCtrl.m_Gradient = iGradient;
}

void CTBListCtrl::SetHeaderBkColor(COLORREF clrBk, int iGradient)
{
	m_headerCtrl.m_Gradient = iGradient;
	m_headerCtrl.m_R = GetRValue(clrBk);
	m_headerCtrl.m_G = GetGValue(clrBk);
	m_headerCtrl.m_B = GetBValue(clrBk);
}


void CTBListCtrl::SetHeaderFontWithHeight(int iWidth, int iHeight)
{
	m_headerCtrl.m_fontWith = iWidth;
	m_headerCtrl.m_fontHeight = iHeight;
}


void CTBListCtrl::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if (pHeader)
	{
		VERIFY(m_headerCtrl.SubclassWindow(pHeader->m_hWnd));
	}
	CListCtrl::PreSubclassWindow();
}


BOOL CTBListCtrl::SetItemButton(int iItem, int iSubItem, CButton* pBtn, int nID, DWORD dwStyle,
	LPCTSTR lpszCaption)
{
	map<int, PITEM_INFO>::iterator it = m_mapItemInfo.find(iItem);
	if (m_mapItemInfo.end() == it)
	{
		return FALSE;
	} 
	else
	{
		list<SUBITEM_INFO>::iterator subIt = it->second->listSubItem.begin();
		list<SUBITEM_INFO>::iterator subItEnd = it->second->listSubItem.end();
		CRect rect;
		VERIFY(GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rect));
		rect.DeflateRect(CSize(1, 1));
		for (; subIt != subItEnd; ++subIt)
		{
			if (iSubItem == subIt->iSubItem)
			{
				ASSERT(pBtn);
				if (subIt->pBtn)
				{
					subIt->pBtn->DestroyWindow();
					delete subIt->pBtn;
				}
				subIt->pBtn = pBtn;
				pBtn->Create(lpszCaption, dwStyle|WS_CHILD|WS_CLIPCHILDREN, rect, this, nID);
				return TRUE;
			}
		}
		SUBITEM_INFO subItemInfo;
		subItemInfo.iSubItem = iSubItem;
		ASSERT(pBtn);
		subItemInfo.pBtn = pBtn;
		pBtn->Create(lpszCaption, dwStyle|WS_CHILD|WS_CLIPCHILDREN, rect, this, nID);
		it->second->listSubItem.push_back(subItemInfo);
		return TRUE;
	}
}


CButton* CTBListCtrl::GetItemButton(int iItem, int iSubItem)
{
	ItemMap::iterator itFind = m_mapItemInfo.find(iItem);
	if (m_mapItemInfo.end() == itFind)
	{
		return NULL;
	}
	else
	{
		list<SUBITEM_INFO>::iterator subIt = itFind->second->listSubItem.begin();
		list<SUBITEM_INFO>::iterator subItEnd = itFind->second->listSubItem.end();
		list<SUBITEM_INFO>::iterator itItem = std::find_if(subIt, subItEnd,
			[iSubItem](list<SUBITEM_INFO>::reference subItem)
		{
			return subItem.iSubItem == iSubItem;
		});
		if (subItEnd == itItem)
		{
			return NULL;
		} 
		else
		{
			return itItem->pBtn;
		}
	}
}


void CTBListCtrl::UpdateComponentPos(BOOL bRepaint)
{
	ItemMap::iterator it = m_mapItemInfo.begin();
	ItemMap::iterator itEnd = m_mapItemInfo.end();

	list<SUBITEM_INFO>::iterator itSub, itSubEnd;
	CRect subItemRect;
	
	while (it != itEnd)
	{
		itSub = it->second->listSubItem.begin();
		itSubEnd = it->second->listSubItem.end();
		while (itSub != itSubEnd)
		{
			if (itSub->pBtn)
			{
				VERIFY(GetSubItemRect(it->first, itSub->iSubItem, LVIR_LABEL, subItemRect));
				subItemRect.DeflateRect(CSize(1, 1));
				
				if (!IsItemVisible(it->first))
				{
					itSub->pBtn->EnableWindow(FALSE);
					itSub->pBtn->ShowWindow(SW_HIDE);
					itSub->pBtn->MoveWindow(&subItemRect, bRepaint);
				}
				else
				{
					itSub->pBtn->MoveWindow(&subItemRect, bRepaint);
					itSub->pBtn->EnableWindow(TRUE);
					itSub->pBtn->ShowWindow(SW_SHOW);
				}
				::SendMessage(itSub->pBtn->m_hWnd, UM_MOVE_COMPONENT, it->first, itSub->iSubItem);
				itSub->pBtn->Invalidate(TRUE);
			}
			++itSub;
		}
		++it;
	}
}


//void CTBListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
//	UpdateComponentPos();
//	InvalidAndUpdateWindow();
//	
//}
//
//
//void CTBListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
//	UpdateComponentPos();
//	InvalidAndUpdateWindow();
//}


BOOL CTBListCtrl::OnNMClick(NMHDR * pNotifyStruct, LRESULT* result)
{
	LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pNotifyStruct;
	ASSERT(lpnmitem);
	
	/*LVHITTESTINFO info;
	info.pt = lpnmitem->ptAction;
	info.flags = LVHT_ONITEM;
	info.iItem = lpnmitem->iItem;
	info.iSubItem = lpnmitem->iSubItem;
	info.iGroup = 0;
	int iIndex = SubItemHitTest(&info);*/
	int iIndex = lpnmitem->iItem;
	if (-1 != iIndex)
	{
		if (!(m_dwExStyle & LVS_EX_FULLROWSELECT))
		{
			if (0 != lpnmitem->iSubItem)
			{
				return FALSE;
			}
		}
		
		ItemMap::iterator itItem = m_mapItemInfo.find(iIndex);
		if (m_mapItemInfo.end() != itItem)
		{
			itItem->second->bSelected = !itItem->second->bSelected;
			/*if (itItem->second->bSelected)
			{
				m_vecSelectedItem.push_back(iIndex);
			} 
			else
			{
				DeleteSelectedItem(iIndex);
			}*/
			
			InvalidAndUpdateItem(iIndex);
		}
		else
		{
			AfxDebugBreak();
		}
	}
	else
	{
		//AfxDebugBreak();
	}
	*result = 0;
	return FALSE;
}


//void CTBListCtrl::OnNMCustomDraw(NMHDR * pNotifyStruct, LRESULT* result)
//{
//	LPNMLVCUSTOMDRAW lpNMCustomDraw = (LPNMLVCUSTOMDRAW)pNotifyStruct;
//	*result = CDRF_DODEFAULT;
//	switch (lpNMCustomDraw->nmcd.dwDrawStage)
//	{
//	case CDDS_PREPAINT:
//		*result = CDRF_NOTIFYITEMDRAW;
//		break;
//	case CDDS_ITEMPREPAINT:
//		*result = CDRF_NOTIFYSUBITEMDRAW;
//		break;
//	case CDDS_SUBITEM|CDDS_ITEMPREPAINT:
//		{
//			UpdateComponentPos();
//			m_headerCtrl.DrawHeader(CDC::FromHandle(lpNMCustomDraw->nmcd.hdc));
//			*result=CDRF_SKIPDEFAULT;
//		}
//		break;
//	default:
//		break;
//	}
//	//if (CDDS_PREPAINT == lpNMCustomDraw->nmcd.dwDrawStage)
//	//{
//	//	UpdateComponentPos();
//	//	//CWnd::FromHandle(lpNMCustomDraw->nmcd.hdr.hwndFrom)->Invalidate(TRUE);
//	//	*result = CDRF_NOTIFYITEMDRAW;
//	//}
//	//return FALSE;
//}


LRESULT CTBListCtrl::OnBnClickedCallBack(WPARAM wParam, LPARAM lParam)
{
	return ::SendMessage(GetParent()->m_hWnd, BN_CLICKED, wParam, lParam);
}


void CTBListCtrl::OnPaint()
{
	Default();
	if (GetItemCount() <= 0)
	{
		CDC* pDC = GetDC();
		int iSaveDC = pDC->SaveDC();
		CRect rc;
		GetWindowRect(&rc);
		ScreenToClient(&rc);
		CRect rcHead;
		ASSERT_VALID(&m_headerCtrl);
		m_headerCtrl.GetItemRect(0, &rcHead);
		rc.top += rcHead.bottom;
		//rc.bottom += 10;
		CBrush brush(DEFAULT_NORMAL_BK_COLOR);
		pDC->FillRect(&rc, &brush);
		pDC->RestoreDC(iSaveDC);
		ReleaseDC(pDC);
		m_headerCtrl.DrawHeader();
	}
}


BOOL CTBListCtrl::GetCheck(int iItem) const
{
	ItemMap::const_iterator itItem = m_mapItemInfo.find(iItem);
	if (m_mapItemInfo.end() == itItem)
	{
		return FALSE;
	}
	else
	{
		return (BOOL)itItem->second->bSelected;
	}
}


//void CTBListCtrl::DeleteSelectedItem(int iIndex)
//{
//	m_vecSelectedItem.erase(
//		std::remove_if(m_vecSelectedItem.begin(), m_vecSelectedItem.end(),
//		[iIndex](vector<int>::reference ref) -> bool
//	{
//		return ref == iIndex;
//	}), m_vecSelectedItem.end());
//}


const vector<int>& CTBListCtrl::GetSelectedItems()
{
	int iItemCount = GetItemCount();
	m_vecSelectedItem.clear();
	for (int i = 0; i < iItemCount; ++i)
	{
		if (GetCheck(i))
		{
			m_vecSelectedItem.push_back(i);
		}
	}
	return m_vecSelectedItem;
}


BOOL CTBListCtrl::SetCheck(_In_ int nItem, _In_ BOOL fCheck /* = TRUE */)
{
	ItemMap::iterator itItem = m_mapItemInfo.find(nItem);
	if (m_mapItemInfo.end() == itItem)
	{
		return FALSE;
	}
	itItem->second->bSelected = fCheck ? true : false;
	/*DeleteSelectedItem(nItem);
	if (fCheck)
	{
		m_vecSelectedItem.push_back(nItem);
	}*/
	return TRUE;
}


BOOL CTBListCtrl::MoveItem(int iItemFrom, int iItemTo)
{
	ItemMap::iterator it = m_mapItemInfo.find(iItemFrom);
	ItemMap::iterator itEnd = m_mapItemInfo.end();
	if (itEnd == it)
	{
		return FALSE;
	}

	// store ITEM_INFO
	PITEM_INFO pFrom = it->second;

	// delete row from list view but not delete ITEM_INFO
	m_mapItemInfo.erase(it);
	ItemMap tmp;
	for (it = m_mapItemInfo.begin(); it != itEnd;)
	{
		if (it->first > iItemFrom)
		{
			PITEM_INFO pItem = it->second;
			int iIndex = it->first-1;
			m_mapItemInfo.erase(it++);
			tmp.insert(std::make_pair(iIndex, pItem));
		}
		else
		{
			++it;
		}
	}
	std::for_each(tmp.begin(), tmp.end(), [&](ItemMap::reference ref)
	{
		m_mapItemInfo.insert(ref);
	});
	VERIFY(__super::DeleteItem(iItemFrom));
	int iItem = InsertItem(iItemTo, pFrom);
	if (-1 == iItem)
	{
		return FALSE;
	} 
	else
	{
		UpdateComponentPos();
		return TRUE;
	}
	//return -1 == InsertItem(iItemTo, pFrom) ? FALSE : TRUE;
}


BOOL CTBListCtrl::OnLvnEndScroll( NMHDR * pNotifyStruct, LRESULT* result)
{
	UpdateComponentPos();
	InvalidAndUpdateWindow();
	return FALSE;
}


DWORD CTBListCtrl::SetExtendedStyle(DWORD dwNewStyle)
{
	DWORD dwPreStyle = __super::SetExtendedStyle(dwNewStyle);
	m_dwExStyle = dwPreStyle | dwNewStyle;
	return dwPreStyle;
}


CString CTBListCtrl::GetItemText(_In_ int nItem, _In_ int nSubItem)
{
	ItemMap::iterator itFind = m_mapItemInfo.find(nItem);
	if (m_mapItemInfo.end() != itFind)
	{
		list<SUBITEM_INFO>::iterator itSubFind = std::find_if(itFind->second->listSubItem.begin(),
			itFind->second->listSubItem.end(),
			[nSubItem](list<SUBITEM_INFO>::reference ref)->bool
		{
			return ref.iSubItem == nSubItem;
		});
		if (itFind->second->listSubItem.end() != itSubFind)
		{
			return itSubFind->strNormal;
		}
	} 
	return _T("");
}