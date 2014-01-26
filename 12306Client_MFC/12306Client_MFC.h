
// 12306Client_MFC.h : 12306Client_MFC 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号

#include "UrlWrapper.h"

// CMy12306Client_MFCApp:
// 有关此类的实现，请参阅 12306Client_MFC.cpp
//

class CMy12306Client_MFCApp : public CWinApp
{
public:
	CMy12306Client_MFCApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	CUrlWrapper* m_pCurlObj;
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMy12306Client_MFCApp theApp;
