#include "stdafx.h"

#include "UrlWrapper.h"


static const char* FireFox = "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:26.0) Gecko/20100101 Firefox/26.0";
static const char* IE7 = "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; InfoPath.2; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET4.0C; .NET4.0E)";

CUrlWrapper::CUrlWrapper(void)
{
	m_pCurlObj = NULL;
	m_response.reserve(10240);
}


CUrlWrapper::~CUrlWrapper(void)
{
	if (m_pCurlObj)
	{
		::curl_easy_cleanup(m_pCurlObj);
		::curl_global_cleanup();
	}
}


bool CUrlWrapper::Init()
{
	if (CURLE_OK != ::curl_global_init(CURL_GLOBAL_ALL)
		|| NULL == (m_pCurlObj = ::curl_easy_init()))
	{
		return false;
	}
	::curl_easy_setopt( m_pCurlObj, CURLOPT_COOKIEFILE, "./cookies.txt");
	::curl_easy_setopt( m_pCurlObj, CURLOPT_COOKIEJAR, "./cookies.txt");
	return true;
}


bool CUrlWrapper::doHttpPost( const char* url, const char* data, long timeout/* = 10*/)
{
	resetOpt();
	//::curl_easy_setopt(m_pCurlObj, CURLOPT_HEADER, withheader);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_URL, url );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_POST, 1 );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_POSTFIELDS, data );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_TIMEOUT, timeout );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_WRITEFUNCTION, CUrlWrapper::processFunc );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_WRITEDATA, this);
	return ::curl_easy_perform(m_pCurlObj) == 0 ? true : false;
}


bool CUrlWrapper::doHttpGet(const char* url, const char* data, long timeout)
{
	size_t urllen = strlen( url );
	size_t datalen = strlen( data );
	char* surl = new char[ urllen + datalen + 1 + 1];
	if(!surl) return false;
	//strncpy( surl, url, urllen );
	strncpy_s(surl, urllen + datalen + 1 + 1, url, urllen);
	//surl[ urllen ] = '?';
	//strcpy( surl + urllen + 1, data );
	strcpy_s(surl + urllen + 1, datalen+1, data);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_URL, surl );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_HTTPGET, 1 );
	//::curl_easy_setopt( m_pCurlObj, CURLOPT_HEADER, withheader );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_TIMEOUT, timeout );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_WRITEFUNCTION, CUrlWrapper::processFunc);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_WRITEDATA, this );
	CURLcode error_code = ::curl_easy_perform(m_pCurlObj);
	delete []surl;
	return 0==error_code ? true : false;
}

bool CUrlWrapper::doHttpsGet(const char* url, long timeout)
{
	resetOpt();
	struct curl_slist *headers = NULL;
	headers = createHeader(headers);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_URL, url );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_SSL_VERIFYPEER, 0);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_SSL_VERIFYHOST, 1);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_HTTPGET, 1 );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_HEADER, headers);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_TIMEOUT, timeout );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_CONNECTTIMEOUT, timeout);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_WRITEFUNCTION, CUrlWrapper::processFunc);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_WRITEDATA, this);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_NOSIGNAL, 1);
	
	CURLcode error_code = ::curl_easy_perform(m_pCurlObj);
	
	curl_slist_free_all(headers);
	return 0==error_code ? true : false;
}

bool CUrlWrapper::doHttpsPost(const char* url, const char* post_data, long timeout/* = 10*/)
{
	resetOpt();
	struct curl_slist *headers = NULL;
	headers = createHeader(headers);
	char header_length[256] = {'\0'};
	strcat_s(header_length, 256, "Content-Length:");
	sprintf_s(header_length+strlen(header_length), 256-strlen(header_length), "%d", strlen(header_length)+strlen(post_data));
	headers = ::curl_slist_append(headers, header_length);
	headers = ::curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded; charset=UTF-8");
	::curl_easy_setopt( m_pCurlObj, CURLOPT_URL, url );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_SSL_VERIFYPEER, 0L);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_SSL_VERIFYHOST, 0L);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_POST, 1 );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_POSTFIELDS, post_data);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_HEADER, headers);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_TIMEOUT, timeout );
	::curl_easy_setopt( m_pCurlObj, CURLOPT_CONNECTTIMEOUT, timeout);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_WRITEFUNCTION, CUrlWrapper::processFunc);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_WRITEDATA, this);
	::curl_easy_setopt( m_pCurlObj, CURLOPT_NOSIGNAL, 1);
	CURLcode error_code = ::curl_easy_perform(m_pCurlObj);
	
	curl_slist_free_all(headers);
	return 0==error_code ? true : false;
}


curl_slist* CUrlWrapper::createHeader(curl_slist *headers)
{
	char* agent = new char[512];
	memset(agent, 0, 512);
	strcpy_s(agent, 512, "User-Agent:");
	strcpy_s(agent + strlen("User-Agent:"), 512 - strlen("User-Agent:"), FireFox);
	headers = ::curl_slist_append(headers, "Host:kyfw.12306.cn");
	headers = ::curl_slist_append(headers, agent);
	headers = ::curl_slist_append(headers, "Accept:*/*");
	headers = ::curl_slist_append(headers, "Accept-Language:zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3");
	headers = ::curl_slist_append(headers, "Accept-Encoding:gzip, deflate");
	headers = ::curl_slist_append(headers, "Connection:keep-alive");
	headers = ::curl_slist_append(headers, "Cache-Control:max-age=0");
	delete[] agent;
	return headers;
}

void CUrlWrapper::resetOpt()
{
	m_response.clear();
	::curl_easy_reset( m_pCurlObj );
	//::curl_easy_setopt( m_pCurlObj, CURLOPT_VERBOSE, 1 );//set this to show debug message
}

size_t CUrlWrapper::processFunc( void* ptr, size_t size, size_t nmemb, void *usrptr )
{
	CUrlWrapper* p =(CUrlWrapper*) usrptr;
	return p->process( ptr, size, nmemb );
}

int CUrlWrapper::process( void* data, size_t size, size_t nmemb )
{
	size_t len = size*nmemb;
	for (size_t i = 0; i < len; i++)
	{
		m_response.push_back(((byte*)data)[i]);
	}
	return len;
}


void CUrlWrapper::getRandom(char* code, size_t sizeOfBuffer)
{
	srand((unsigned)time(NULL));
	double dcode = (double)rand() / (RAND_MAX+1);
	sprintf_s(code, sizeOfBuffer, "%0.16f", dcode);
}


size_t CUrlWrapper::getResponseData(void* data, size_t len)
{
	size_t length = m_response.size();
	if(!length)
		return 0;
	ASSERT_POINTER(data, byte);
	if(0==memcpy_s(data, len, m_response.data(), length))
	{
		return length;
	}
	else
	{
		return 0;
	}
}

size_t CUrlWrapper::getResponseLength()
{
	return m_response.size();
}

