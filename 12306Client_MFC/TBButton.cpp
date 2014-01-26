// TBButton.cpp : 实现文件
//

#include "stdafx.h"
#include "12306Client_MFC.h"
#include "TBButton.h"


// CTBButton

IMPLEMENT_DYNAMIC(CTBButton, CButton)

CTBButton::CTBButton(DWORD dwItem, DWORD dwSubItem)
{
	m_dwItem = dwItem;
	m_dwSubItem = dwSubItem;
}

CTBButton::~CTBButton()
{
}


BEGIN_MESSAGE_MAP(CTBButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CTBButton::OnBnClicked)
	ON_MESSAGE(UM_MOVE_COMPONENT, &CTBButton::OnMove)
END_MESSAGE_MAP()



// CTBButton 消息处理程序
void CTBButton::OnBnClicked()
{
	::SendMessage(GetParent()->m_hWnd, BN_CLICKED, m_dwItem, m_dwSubItem);
}


LRESULT CTBButton::OnMove(WPARAM wParam, LPARAM lParam)
{
	m_dwItem = wParam;
	m_dwSubItem = lParam;
	return 1;
}

