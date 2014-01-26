#pragma once


// CTBColorButton

class CTBColorButton : public CMFCColorButton
{
	DECLARE_DYNAMIC(CTBColorButton)

public:
	CTBColorButton(DWORD dwItem, DWORD dwSubItem);
	virtual ~CTBColorButton();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClicked();
	afx_msg LRESULT OnMove(WPARAM wParam, LPARAM lParam);

private:
	DWORD m_dwItem;
	DWORD m_dwSubItem;
};


