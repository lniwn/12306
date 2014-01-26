#pragma once
#include <vector>

extern "C"
{
#include <curl/curl.h>
}
class CUrlWrapper
{
public:
	CUrlWrapper(void);
	~CUrlWrapper(void);

	bool Init();
	bool doHttpPost( const char* url, const char* data, long timeout = 10);
	bool doHttpGet( const char* url, const char* data, long timeout = 10);
	bool doHttpsGet(const char* url, long timeout = 10);
	bool doHttpsPost(const char* url, const char* post_data, long timeout = 10);
	void resetOpt();
	void getRandom(char* code, size_t sizeOfBuffer);
	size_t getResponseData(void* data, size_t len);
	size_t getResponseLength();
private:
	CURL* m_pCurlObj;
	curl_slist* createHeader(curl_slist *headers);
	int process(void* data, size_t size, size_t nmemb);
	static size_t processFunc( void* ptr, size_t size, size_t nmemb, void *usrptr );
	std::vector<byte> m_response;
};

