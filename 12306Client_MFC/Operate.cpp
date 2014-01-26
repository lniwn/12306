#include "stdafx.h"
#include "Operate.h"

int utf8ToUnicode(const char* src, int src_len, wchar_t* dst)
{
	int wcs_len = ::MultiByteToWideChar(CP_UTF8, 0, src, src_len, NULL, 0);
	if(0 == wcs_len) return 0;
	return ::MultiByteToWideChar(CP_UTF8, 0, src, src_len, dst, wcs_len);
}

int unicodeToUtf8(const wchar_t* src, int src_len, char* dst)
{
	int utf8_len = ::WideCharToMultiByte(CP_UTF8, 0, src, src_len, NULL, 0, nullptr, nullptr);
	if(0 == utf8_len) return 0;
	return ::WideCharToMultiByte(CP_UTF8, 0, src, src_len, dst, utf8_len, nullptr, nullptr);
}

HGLOBAL CreateStreamFromMemory(void* src, size_t len, IStream** ppStream)
{
	HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, len);
	ASSERT(hGlobal);
	LPVOID pData = ::GlobalLock(hGlobal);
	ASSERT(pData);
	CopyMemory(pData, src, len);
	VERIFY(SUCCEEDED(::CreateStreamOnHGlobal(hGlobal, TRUE, ppStream)));
	::GlobalUnlock(hGlobal);
	return hGlobal;
}