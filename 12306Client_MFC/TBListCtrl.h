#pragma once
#include <list>
#include <map>
#include <vector>
#include "TBButton.h"

using std::map;
using std::list;
using std::vector;

// move component, WPARAM: row number, LPARAM: col number
//#define UM_MOVE_COMPONENT WM_USER+101

#include "TBHeaderCtrl.h"


//#define DEFAULT_TITLE_BK_COLOR RGB(0, 0, 0)
//#define DEFAULT_TITLE_TEXT_COLOR RGB(255, 0, 0)
#define DEFAULT_NORMAL_BK_COLOR RGB(255, 255, 255)
#define DEFAULT_NORMAL_TEXT_COLOR RGB(0, 0, 0)

typedef struct tagSUBITEM_INFO
{
	int iSubItem; 
	COLORREF clrTextColor; 
	COLORREF clrBkColor;
	CString strNormal; // item text
	CButton* pBtn;
}SUBITEM_INFO, *PSUBITEM_INFO;

typedef struct tagITEM_INFO
{
	//DWORD dwFlags; // LISTITEM_TITLE
	//CString strBold; 
	//CString strNormal; 
	//COLORREF clrBoldColor; 
	COLORREF clrNormalColor; 
	COLORREF clrBkColor;
	bool bSelected;
	list<SUBITEM_INFO> listSubItem; 
}ITEM_INFO, *PITEM_INFO;

// CTBListCtrl

class CTBListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CTBListCtrl)
	typedef map<int, PITEM_INFO> ItemMap;
public:
	CTBListCtrl();
	virtual ~CTBListCtrl();

// Method
public:
	virtual BOOL DeleteAllItems();
	virtual BOOL DeleteItem(_In_ int nItem);
	virtual BOOL DeleteColumn(_In_ int nCol);
	BOOL SetItemText(_In_ int nItem, _In_ int nSubItem, _In_z_ LPCTSTR lpszText, COLORREF clrTextColor = DEFAULT_NORMAL_TEXT_COLOR);
	BOOL SetItemTextColor(int iItem, int iSubItem, COLORREF clrColor);
	BOOL SetItemBkColor(int iItem, COLORREF clrBkColor);
	void SetRowHeight(int iItemHeight = 0x20);
	void InvalidAndUpdateWindow();
	void InvalidAndUpdateItem(int iItem);
	int InsertItem(int iItem, LPCTSTR lpszItem, COLORREF clrBkColor = DEFAULT_NORMAL_BK_COLOR,
		COLORREF clrTextColor = DEFAULT_NORMAL_TEXT_COLOR);
	int InsertItem(int iItem, PITEM_INFO pItemInfo);
	BOOL SetItemButton(int iItem, int iSubItem, CButton* pBtn, int nID, DWORD dwStyle = 0,
		LPCTSTR lpszCaption = _T(""));
	CButton* GetItemButton(int iItem, int iSubItem);
	void SetLineColor(COLORREF clrLineColor);
	void SetSelectedRowColor(COLORREF clrRow);
	BOOL InitListCtrl();
	void SetFontWidthHeight(DWORD dwWidth, DWORD dwHeight);
	void UpdateComponentPos(BOOL bRepaint = TRUE);
	BOOL GetCheck(int iItem) const;
	const std::vector<int>& GetSelectedItems();
	BOOL SetCheck(_In_ int nItem, _In_ BOOL fCheck = TRUE);
	BOOL MoveItem(int iItemFrom, int iItemTo);
	DWORD SetExtendedStyle(DWORD dwNewStyle);
	CString GetItemText(_In_ int nItem, _In_ int nSubItem);
	void SetHeaderHeightRatio(float fRatio);
	void SetHeaderBkColor(int R, int G, int B, int iGradient=1);
	void SetHeaderBkColor(COLORREF clrBk, int iGradient=1);
	void SetHeaderFontColor(COLORREF clrFont);
	void SetHeaderFontWithHeight(int iWidth, int iHeight);
	BOOL IsItemVisible(int iIndex);

private:
	DWORD m_dwRowHeight;
	COLORREF m_clrLineColor;
	COLORREF m_clrSelectedRowColor;
	ItemMap m_mapItemInfo;
	DWORD m_dwFontWidth, m_dwFontHeight;
	static HHOOK m_hHook;
	static CWnd* m_pWnd;
	DWORD m_dwExStyle;
	vector<int> m_vecSelectedItem;

	void GetFirstItemRect(int iItem, LPRECT pRect);
	void DrawNormalItem(LPDRAWITEMSTRUCT lpDrawItem);
	COLORREF GetItemTextColor(int iItem, int iSubItem);
	COLORREF GetItemBkColor(int iItem);
	void ClearMapInfo();
	//void DeleteSelectedItem(int);
	
	static LRESULT CALLBACK CBTProc(int code,WPARAM wParam,LPARAM lParam);
	CTBHeaderCtrl m_headerCtrl;


protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	DECLARE_MESSAGE_MAP()
public:
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar); 
	//afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnLvnEndScroll( NMHDR * pNotifyStruct, LRESULT* result); 
	afx_msg BOOL OnNMClick( NMHDR * pNotifyStruct, LRESULT* result); 
	//afx_msg void OnNMCustomDraw( NMHDR * pNotifyStruct, LRESULT* result);
	afx_msg void OnPaint();

	// wParam: item
	// lParam: subItem
	afx_msg LRESULT OnBnClickedCallBack(WPARAM wParam, LPARAM lParam); 

};


