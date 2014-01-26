// MainView.cpp : 实现文件
//

#include "stdafx.h"
#include "12306Client_MFC.h"
#include "afxdialogex.h"
#include <json\json.h>
#include "Operate.h"
#include "MainFrm.h"
#include "MainView.h"

#define LOG(_l) ((CMainFrame*)m_pParentWnd)->ShowLog(_l)

static UINT _BUTTON_ID = 10112;
#define BUTTON_ID _BUTTON_ID++

// CMainView 对话框

IMPLEMENT_DYNAMIC(CMainView, CDialogEx)

CMainView::CMainView(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainView::IDD, pParent)
{
	m_pCityBuffer = NULL;
	m_lCityBuffer = 0;
}

CMainView::~CMainView()
{
	if(m_pCityBuffer)
	{
		delete[] m_pCityBuffer;
		m_pCityBuffer = NULL;
		
	}
}

void CMainView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TRAIN, m_lctrlMain);
}


BEGIN_MESSAGE_MAP(CMainView, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CMainView::OnBnClickedButtonQuery)
	ON_WM_SIZE()
	ON_MESSAGE(BN_CLICKED, &CMainView::OnBnClickedBook)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMainView 消息处理程序


void CMainView::OnBnClickedButtonQuery()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strStart, strEnd;
	GetDlgItemText(IDC_EDIT_START, strStart);
	GetDlgItemText(IDC_EDIT_END, strEnd);
	COleDateTime targetTime;
	((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_START))->GetTime(targetTime);

	char url[512] = {'\0'};
	int offset = 0;
	strcat_s(url, 512, "https://kyfw.12306.cn/otn/leftTicket/query?leftTicketDTO.train_date=");
	offset = strlen(url);
	// https://kyfw.12306.cn/otn/leftTicket/query?leftTicketDTO.train_date=2014-01-23&
	// leftTicketDTO.from_station=SHH&leftTicketDTO.to_station=TYV&purpose_codes=ADULT

	CString strTime = targetTime.Format(L"%Y-%m-%d");
	size_t dwConverted;
	::wcstombs_s(&dwConverted, url+offset, 512-offset, strTime, strTime.GetLength()*sizeof(TCHAR));
	m_strDate.clear();
	m_strDate.append(url+offset);
	offset = strlen(url);
	strcat_s(url+offset, 512-offset, "&leftTicketDTO.from_station=");
	offset = strlen(url);
	CString strCityCode;
	if(!GetCityCode(strStart, strCityCode))
	{
		LOG(L"无法获取始发站信息！");
		return;
	}
	::wcstombs_s(&dwConverted, url+offset, 512-offset, strCityCode, strCityCode.GetLength()*sizeof(TCHAR));
	offset = strlen(url);
	strcat_s(url+offset, 512-offset, "&leftTicketDTO.to_station=");
	offset = strlen(url);
	if(!GetCityCode(strEnd, strCityCode))
	{
		LOG(L"无法获取终点站信息！");
		return;
	}
	::wcstombs_s(&dwConverted, url+offset, 512-offset, strCityCode, strCityCode.GetLength()*sizeof(TCHAR));
	offset = strlen(url);
	strcat_s(url+offset, 512-offset, "&purpose_codes=ADULT");
	if(!m_pCurlObj->doHttpsGet(url))
	{
		LOG(L"无法获取车次信息！");
		return;
	}
	offset = m_pCurlObj->getResponseLength();
	char* buff = new char[offset];
	memset(buff, '\0', offset);
	m_pCurlObj->getResponseData(buff, offset);
	char* begin = strstr(buff, "\r\n\r\n");
	if(begin == NULL)
	{
		LOG(L"无法解析12306的返回值！");
		return;
	}
	begin = strstr(begin, "{");
	if(begin == NULL)
	{
		LOG(L"无法解析12306的返回值！");
		return;
	}
	ShowTrainInfo(begin, strlen(begin));
	
	delete[] buff;
	
}


void CMainView::ShowTrainInfo(char* buffer, int len)
{
	ClearItemData();
	m_lctrlMain.DeleteAllItems();
	Json::Reader JReader;
	Json::Value JObject;
	if(!JReader.parse(buffer, buffer+len, JObject, false)) return;
	Json::Value data = JObject["data"];
	Json::UInt count = data.size();
	Json::Value defaultValue = Json::Value::null;
	
	for(Json::UInt i = 0; i < count; ++i)
	{
		Json::Value queryLeftNewDTO = data[i]["queryLeftNewDTO"];
		if(queryLeftNewDTO.isNull())
		{
			continue;
		}
		PTICKETITEM ticket = new TICKETITEM;
		
		ticket->station_train_code	= queryLeftNewDTO.get("station_train_code", defaultValue).asString();
		ticket->start_station_name	= queryLeftNewDTO.get("start_station_name", defaultValue).asString();
		ticket->end_station_name	= queryLeftNewDTO.get("end_station_name", defaultValue).asString();
		ticket->from_station_name	= queryLeftNewDTO.get("from_station_name", defaultValue).asString();
		ticket->to_station_name		= queryLeftNewDTO.get("to_station_name", defaultValue).asString();
		ticket->start_time			= queryLeftNewDTO.get("start_time", defaultValue).asString();
		ticket->arrive_time			= queryLeftNewDTO.get("arrive_time", defaultValue).asString();
		ticket->day_difference		= queryLeftNewDTO.get("day_difference", defaultValue).asString();
		ticket->lishi				= queryLeftNewDTO.get("lishi", defaultValue).asString();
		ticket->canWebBuy			= queryLeftNewDTO.get("canWebBuy", defaultValue).asBool();
		ticket->lishiValue			= queryLeftNewDTO.get("lishiValue", defaultValue).asString();
		ticket->yp_info				= queryLeftNewDTO.get("yp_info", defaultValue).asString();
		ticket->seat_feature		= queryLeftNewDTO.get("seat_feature", defaultValue).asString();
		ticket->yp_ex				= queryLeftNewDTO.get("yp_ex", defaultValue).asString();
		ticket->swz_num				= queryLeftNewDTO.get("swz_num", defaultValue).asString();
		ticket->tz_num				= queryLeftNewDTO.get("tz_num", defaultValue).asString();
		ticket->zy_num				= queryLeftNewDTO.get("zy_num", defaultValue).asString();
		ticket->ze_num				= queryLeftNewDTO.get("ze_num", defaultValue).asString();
		ticket->gr_num				= queryLeftNewDTO.get("gr_num", defaultValue).asString();
		ticket->rw_num				= queryLeftNewDTO.get("rw_num", defaultValue).asString();
		ticket->yw_num				= queryLeftNewDTO.get("yw_num", defaultValue).asString();
		ticket->rz_num				= queryLeftNewDTO.get("rz_num", defaultValue).asString();
		ticket->yz_num				= queryLeftNewDTO.get("yz_num", defaultValue).asString();
		ticket->wz_num				= queryLeftNewDTO.get("wz_num", defaultValue).asString();
		ticket->qt_num				= queryLeftNewDTO.get("qt_num", defaultValue).asString();
		ticket->secretStr			= queryLeftNewDTO.get("secretStr", defaultValue).asString();
		ticket->buttonTextInfo		= data[i].get("buttonTextInfo", defaultValue).asString();
		
		wchar_t* wbuf = new wchar_t[512];
		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->station_train_code.c_str(), ticket->station_train_code.size(), wbuf);
		m_lctrlMain.InsertItem(i, wbuf);

		int wsize = utf8ToUnicode(ticket->from_station_name.c_str(), ticket->from_station_name.size(), wbuf);
		wcscpy_s(wbuf+wsize, wcslen(L"->")+1, L"->");
		utf8ToUnicode(ticket->to_station_name.c_str(), ticket->to_station_name.size(), wbuf+wcslen(wbuf));
		m_lctrlMain.SetItemText(i, 1, wbuf);

		memset(wbuf, 0, 512);
		wsize = utf8ToUnicode(ticket->start_time.c_str(), ticket->start_time.size(), wbuf);
		wcscpy_s(wbuf+wsize, wcslen(L"->")+1, L"->");
		utf8ToUnicode(ticket->arrive_time.c_str(), ticket->arrive_time.size(), wbuf+wcslen(wbuf));
		m_lctrlMain.SetItemText(i, 2, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->lishi.c_str(), ticket->lishi.size(), wbuf);
		m_lctrlMain.SetItemText(i, 3, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->swz_num.c_str(), ticket->swz_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 4, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->tz_num.c_str(), ticket->tz_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 5, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->zy_num.c_str(), ticket->zy_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 6, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->ze_num.c_str(), ticket->ze_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 7, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->gr_num.c_str(), ticket->gr_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 8, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->rw_num.c_str(), ticket->rw_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 9, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->yw_num.c_str(), ticket->yw_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 10, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->rz_num.c_str(), ticket->rz_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 11, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->yz_num.c_str(), ticket->yz_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 12, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->wz_num.c_str(), ticket->wz_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 13, wbuf);

		memset(wbuf, 0, 512);
		utf8ToUnicode(ticket->qt_num.c_str(), ticket->qt_num.size(), wbuf);
		m_lctrlMain.SetItemText(i, 14, wbuf);

		if(ticket->canWebBuy)
		{
			memset(wbuf, 0, 512);
			utf8ToUnicode(ticket->buttonTextInfo.c_str(), ticket->buttonTextInfo.size(), wbuf);
			m_lctrlMain.SetItemButton(i, 15, new CTBButton(i, 15), BUTTON_ID, WS_VISIBLE|BS_NOTIFY|BS_TEXT, wbuf);
		}
		
		delete[] wbuf;
		m_lctrlMain.SetItemData(i, reinterpret_cast<DWORD_PTR>(ticket));
	}
}


BOOL CMainView::GetCityCode(LPCTSTR lpszCity, CString& strCode)
{
	if(!m_pCityBuffer || m_lCityBuffer == 0)
	{
		// 从本地文件载入city.txt文件
		/*FILE* hFile;
		if(0!=_tfopen_s(&hFile, L"./city.txt", L"rb")) return FALSE;
		fseek(hFile, 0, SEEK_END);
		m_lCityBuffer = ftell(hFile);
		fseek(hFile, 0, SEEK_SET);
		char* buff = new char[m_lCityBuffer];
		memset(buff, 0, m_lCityBuffer);
		fread_s(buff, m_lCityBuffer*sizeof(char), sizeof(char), m_lCityBuffer, hFile);
		fclose(hFile);*/

		// 从资源文件载入city.txt文件
		HRSRC hTxt = ::FindResource(NULL, MAKEINTRESOURCE(IDR_TXT_CITY), L"TXT");
		if(!hTxt) return FALSE;
		m_lCityBuffer = ::SizeofResource(NULL, hTxt);
		HGLOBAL hGlobal = ::LoadResource(NULL, hTxt);
		if(!hGlobal) return FALSE;
		char* buff = (char*)::LockResource(hGlobal);
		m_pCityBuffer = new TCHAR[m_lCityBuffer];
		memset(m_pCityBuffer, 0, m_lCityBuffer);
		utf8ToUnicode(buff, m_lCityBuffer, m_pCityBuffer);
		//delete[] buff;
		::UnlockResource(hGlobal);
		::FreeResource(hGlobal);

		m_mapCity.InitHashTable(2114);
	}
	if(m_mapCity.Lookup(CString(lpszCity), strCode)) return TRUE;
	LPTSTR lpTarget = StrStrI(m_pCityBuffer, lpszCity);
	if(NULL == lpTarget) return FALSE;
	LPTSTR lpFirst = StrStrI(lpTarget, L"|");
	if(NULL == lpFirst) return FALSE;
	++lpFirst;
	LPTSTR lpSecond = StrStrI(lpFirst, L"|");
	if(NULL == lpSecond) return FALSE;
	if(lpSecond - lpFirst< 2) return FALSE;
	TCHAR* code_buf = new TCHAR[lpSecond - lpFirst + 1];
	ZeroMemory(code_buf, lpSecond - lpFirst + 1);
	_tcsncpy_s(code_buf, lpSecond - lpFirst + 1, lpFirst, lpSecond - lpFirst);
	strCode = code_buf;
	delete[] code_buf;
	m_mapCity[lpszCity] = strCode;
	return TRUE;
}

BOOL CMainView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT(theApp.m_pCurlObj);
	m_pCurlObj = theApp.m_pCurlObj;

	m_lctrlMain.InitListCtrl();
	
	m_lctrlMain.InsertColumn(0, L"车次", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(1, L"起/终站", LVCFMT_LEFT, 200);
	m_lctrlMain.InsertColumn(2, L"起/终时间", LVCFMT_LEFT, 180);
	m_lctrlMain.InsertColumn(3, L"历时", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(4, L"商务座", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(5, L"特等座", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(6, L"一等座", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(7, L"二等座", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(8, L"高级软卧", LVCFMT_LEFT, 80);
	m_lctrlMain.InsertColumn(9, L"软卧", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(10, L"硬卧", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(11, L"软座", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(12, L"硬座", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(13, L"无座", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(14, L"其他", LVCFMT_LEFT, 50);
	m_lctrlMain.InsertColumn(15, L"备注", LVCFMT_LEFT, 80);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CMainView::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(::IsWindowVisible(m_lctrlMain.GetSafeHwnd()))
	{
		CRect oldRc;
		m_lctrlMain.GetWindowRect(&oldRc);
		ScreenToClient(&oldRc);
		m_lctrlMain.MoveWindow(oldRc.left, oldRc.top, cx - oldRc.left, cy - oldRc.top, FALSE);
	}
}


LRESULT CMainView::OnBnClickedBook(WPARAM wParam, LPARAM lParam)
{
	char url[512] = {'\0'};
	strcat_s(url, 512, "https://kyfw.12306.cn/otn/login/checkUser");
	Json::Reader jReader;
	Json::Value jObject;
	m_pCurlObj->doHttpsPost(url, "_json_att=");
	size_t res_len = m_pCurlObj->getResponseLength();
	char *response = new char[res_len];
	memset(response, 0, res_len);
	m_pCurlObj->getResponseData(response, res_len);
	char* begin = strstr(response, "\r\n\r\n");
	if(begin == NULL)
	{
		LOG(L"无法解析12306的返回值！");
		delete[] response;
		return 0;
	}
	begin = strstr(begin, "{");
	if(begin == NULL)
	{
		LOG(L"无法解析12306的返回值！");
		delete[] response;
		return 0;
	}
	if(!jReader.parse(begin, begin+strlen(begin), jObject, false))
	{
		LOG(L"无法解析12306的返回值！");
		delete[] response;
		return 0;
	}
	
	if(0!=_strcmpi("true", jObject["data"]["flag"].asString().data()))
	{
		LOG(L"验证用户失败！");
		delete[] response;
		return 0;
	}

	memset(url, 0, 512);
	strcat_s(url, 512, "https://kyfw.12306.cn/otn/leftTicket/submitOrderRequest");
	char* post_fields = new char[512];
	memset(post_fields, 0, 512);
	strcat_s(post_fields, 512, "secretStr=");
	PTICKETITEM pItem = reinterpret_cast<PTICKETITEM>(m_lctrlMain.GetItemData(wParam));
	if(!pItem)
	{
		delete[] post_fields;
		delete[] response;
		return 0;
	}
	strcat_s(post_fields, 512 - strlen(post_fields), pItem->secretStr.data());
	strcat_s(post_fields, 512 - strlen(post_fields), "&train_date=");
	strcat_s(post_fields, 512 - strlen(post_fields), m_strDate.data());
	strcat_s(post_fields, 512 - strlen(post_fields), "&back_train_date=");
	strcat_s(post_fields, 512 - strlen(post_fields), m_strDate.data());
	strcat_s(post_fields, 512 - strlen(post_fields), "&tour_flag=dc&purpose_codes=ADULT&query_from_station_name=");
	strcat_s(post_fields, 512 - strlen(post_fields), pItem->from_station_name.data());
	strcat_s(post_fields, 512 - strlen(post_fields), "&query_to_station_name=");
	strcat_s(post_fields, 512 - strlen(post_fields), pItem->to_station_name.data());
	strcat_s(post_fields, 512 - strlen(post_fields), "&undefined");
	if(m_pCurlObj->doHttpsPost(url, post_fields))
	{
		memset(url, 0, 512);
		strcat_s(url, 512, "https://kyfw.12306.cn/otn/confirmPassenger/initDc");
		if(m_pCurlObj->doHttpsPost(url, "_json_att="))
		{
			/*CString strCode;
			GetDlgItemText(IDC_EDIT_ORDER_RAND, strCode);
			if(strCode.IsEmpty())
			{
				LOG(L"请输入验证码!");
				return;
			}
			LOG(L"校验验证码...");
			size_t converted;
			::wcstombs_s(&converted, post_fields+strlen(post_fields),128-strlen(post_fields), strCode, strCode.GetLength()*sizeof(TCHAR));*/
			// 从这里开始就没写了。。。
			res_len = m_pCurlObj->getResponseLength();
			memset(response, 0, res_len);
			m_pCurlObj->getResponseData(response, res_len);
			begin = strstr(response, "globalRepeatSubmitToken");
			begin = strstr(begin, "'");
			std::string globalRepeatSubmitToken(begin+1, strstr(begin+1, "'"));
			memset(url, 0, 512);
			strcat_s(url, 512, "https://kyfw.12306.cn/otn/passcodeNew/checkRandCodeAnsyn");
			memset(post_fields, 0, 512);
		}
	}

	delete[] post_fields;
	delete[] response;
	return 0;
}

BOOL CMainView::GetRandCode()
{
	LOG(L"获取验证码...");
	char url[512] = {0};
	strcat_s(url, 512, "https://kyfw.12306.cn/otn/passcodeNew/getPassCodeNew.do?module=passenger&rand=randp&");
	char code[30] = {0};
	m_pCurlObj->getRandom(code, 30);
	strcat_s(url, 512 - strlen(url), code);
	if(m_pCurlObj->doHttpsGet(url))
	{
		size_t len = m_pCurlObj->getResponseLength();
		
		if(len)
		{
			byte* const data = new byte[len];
			memset(data, '\0', len);
			m_pCurlObj->getResponseData(data, len);
			byte* png = data;
			// 去掉Response Header
			while(isascii(*png++));
			--png;
			IStream* pStream;
			HGLOBAL hGlobal = CreateStreamFromMemory(png, len, &pStream);
			CImage img;
			if(SUCCEEDED(img.Load(pStream)))
			{
				//((CStatic*)GetDlgItem(IDC_STATIC_ORDER_RAND))->SetBitmap(img);
				LOG(L"获取验证码成功！");
				::GlobalFree(hGlobal);
				img.Detach();
				delete[] data;
				return TRUE;
			}
			::GlobalFree(hGlobal);
			delete[] data;
		}
	}
	LOG(L"获取验证码失败！");
	return FALSE;
}

void CMainView::ClearItemData(int item)
{
	ASSERT(m_lctrlMain.IsWindowEnabled());
	int counts = m_lctrlMain.GetItemCount();
	if(item < 0)
	{
		for (int i = 0; i < counts; i++)
		{
			delete reinterpret_cast<PTICKETITEM>(m_lctrlMain.GetItemData(i));
		}
	}
	else if(item < counts)
	{
		delete reinterpret_cast<PTICKETITEM>(m_lctrlMain.GetItemData(item));
	}
}

void CMainView::OnClose()
{
	ClearItemData();

	CDialogEx::OnClose();
}
