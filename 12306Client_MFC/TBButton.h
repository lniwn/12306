#pragma once

#ifndef UM_MOVE_COMPONENT
#define UM_MOVE_COMPONENT WM_USER+101
#endif
// CTBButton

class CTBButton : public CButton
{
	DECLARE_DYNAMIC(CTBButton)

public:
	CTBButton(DWORD dwItem, DWORD dwSubItem);
	virtual ~CTBButton();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClicked();
	afx_msg LRESULT OnMove(WPARAM wParam, LPARAM lParam);

private:
	DWORD m_dwItem;
	DWORD m_dwSubItem;
};


