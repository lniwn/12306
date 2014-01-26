#pragma once


// CHeaderCtrlCl

class CTBHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CTBHeaderCtrl)

public:
	CTBHeaderCtrl();
	virtual ~CTBHeaderCtrl();
private:
	CListCtrl* m_pListCtrl;

protected:
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnPaint();

private:
	friend class CTBListCtrl;
	int m_R;
	int m_G;
	int m_B;
	int m_Gradient;
	float m_Height;
	int m_fontHeight; 
	int m_fontWith; 
	COLORREF m_color;
	LRESULT OnLayout( WPARAM wParam, LPARAM lParam );
	void SetListCtrl(CListCtrl*);
	void DrawHeader(CDC* pDC = NULL);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
};


