// TBColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "TBColorButton.h"


#ifndef UM_MOVE_COMPONENT
#define UM_MOVE_COMPONENT WM_USER+101
#endif

// CTBColorButton

IMPLEMENT_DYNAMIC(CTBColorButton, CMFCColorButton)

CTBColorButton::CTBColorButton(DWORD dwItem, DWORD dwSubItem)
{
	m_dwItem = dwItem;
	m_dwSubItem = dwSubItem;
}

CTBColorButton::~CTBColorButton()
{
}


BEGIN_MESSAGE_MAP(CTBColorButton, CMFCColorButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CTBColorButton::OnBnClicked)
	ON_MESSAGE(UM_MOVE_COMPONENT, &CTBColorButton::OnMove)
END_MESSAGE_MAP()



// CTBColorButton message handlers

void CTBColorButton::OnBnClicked()
{
	::SendMessage(GetParent()->m_hWnd, BN_CLICKED, m_dwItem, m_dwSubItem);
}


LRESULT CTBColorButton::OnMove(WPARAM wParam, LPARAM lParam)
{
	m_dwItem = wParam;
	m_dwSubItem = lParam;
	return 1;
}

