#pragma once
#include "resource.h"
#include <string>
#include "TBListCtrl.h"

typedef struct _TicketItem
{
	std::string station_train_code; // 车次
	std::string start_station_name; // 始发站
	std::string end_station_name;	// 终点站
	std::string from_station_name;	// 起始站
	std::string to_station_name;	// 到达站
	std::string start_time;			// 起始时间
	std::string arrive_time;		// 到达时间
	std::string day_difference;		// 天数
	std::string lishi;				// 经过时间
	bool canWebBuy;					// 是否可以网络购票
	std::string lishiValue;			// 经过时间(分钟)
	std::string yp_info;			// 余票信息
	std::string seat_feature;		// 座位特点(做嘛用的？？)
	std::string yp_ex;				// 余票扩展信息
	std::string swz_num;			// 商务座
	std::string tz_num;				// 特等座
	std::string zy_num;				// 一等座 zuo yi
	std::string ze_num;				// 二等座 zuo er
	std::string gr_num;				// 高级软卧
	std::string rw_num;				// 软卧
	std::string yw_num;				// 硬卧
	std::string rz_num;				// 软座
	std::string yz_num;				// 硬座
	std::string wz_num;				// 无座
	std::string qt_num;				// 其他
	std::string buttonTextInfo;		// 按钮文字
	std::string secretStr;			// 订票标识符
}TICKETITEM, *PTICKETITEM;

// CMainView 对话框
class CUrlWrapper;
class CMainView : public CDialogEx
{
	DECLARE_DYNAMIC(CMainView)

public:
	CMainView(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainView();
	BOOL GetCityCode(LPCTSTR lpszCity, CString& strCode);

// 对话框数据
	enum { IDD = IDD_DIALOG_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	void ShowTrainInfo(char* buffer, int len);
	void ClearItemData(int item = -1);
	BOOL GetRandCode();

private:
	CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapCity;
	TCHAR* m_pCityBuffer;
	DWORD m_lCityBuffer;
	CUrlWrapper* m_pCurlObj;
	std::string m_strDate;

public:
	CTBListCtrl m_lctrlMain;
	afx_msg void OnBnClickedButtonQuery();

	// wParam: item
	// lParam: subItem
	afx_msg LRESULT OnBnClickedBook(WPARAM wParam, LPARAM lParam); 

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
};
