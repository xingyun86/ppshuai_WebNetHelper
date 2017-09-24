// WebnetHelper.cpp : Defines the entry point for the console application.
//

#include <CommonHeader.h>
#include <sys/timeb.h>
#define random(x) (rand()%x)
const char * pChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz/\\";

#define URL_FILENAME			"url"
#define POSTFIELDS_FILENAME		"postfields"
#define HEADERDATA_FILENAME		"headerdata"

#define SCRIPT_FILEPATH			"script\\"
#define SCRIPT_COUNTER			"counter.lst"
#define SCRIPT_DELAYTIME		"delaytime.lst"

std::string current_time()
{
	struct timeb tb = { 0 };
	ftime(&tb);
	char szTime[MAXCHAR] = { 0 };
	switch (sizeof(tb.time))
	{
	case 4:
	{
		_snprintf(szTime, sizeof(szTime) / sizeof(*szTime), "%ld%03u", tb.time, tb.millitm);
	}
	break;
	case 8:
	{
		_snprintf(szTime, sizeof(szTime) / sizeof(*szTime), "%lld%03u", tb.time, tb.millitm);
	}
	break;
	default:
	{
		_snprintf(szTime, sizeof(szTime) / sizeof(*szTime), "%ld%03u", tb.time, tb.millitm);
	}
		break;
	}
	
	return std::string(szTime);
}
BOOL IsFileExists(const char * pName)
{
	WIN32_FILE_ATTRIBUTE_DATA wfad = { 0 };

	return (GetFileAttributesExA(pName, GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &wfad) 
		? ((wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) : FALSE);
}
BOOL IsPathExists(const char * pName)
{
	WIN32_FILE_ATTRIBUTE_DATA wfad = { 0 };
	return (GetFileAttributesExA(pName, GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &wfad)
		? !((wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) : FALSE);
}
std::string read_file(std::string strFileName)
{
	std::string strData = "";
	long lFileSize = 0;
	FILE * pFile = fopen(strFileName.c_str(), "rb");
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		lFileSize = ftell(pFile);
		if (lFileSize > 0)
		{
			fseek(pFile, 0, SEEK_SET);
			strData.resize(lFileSize * sizeof(unsigned char));
			fread((void *)strData.c_str(), strData.size(), sizeof(unsigned char), pFile);
		}
		fclose(pFile);
	}
	return strData;
}

typedef int(*PREQUEST_HANDLER)(std::string &, std::string &, std::string &);
typedef int(*PRESPONSE_HANDLER)(std::string &);
int get_request_handler(string &strRequestURL, string &strHeaderData, string &strPostFields)
{
	int result = 0;
	struct timeb tb = { 0 };
	
	ftime(&tb);
	srand(tb.time);
	std::string strUrlTime = "XXXX-XX-XXTXX%3AXX%3AXX.XXXXXX%2B0000";
	std::string STRDATETIME = TToA(T2S(tb.time, TFTYPE_0));
	_snprintf((char *)strUrlTime.c_str(), strUrlTime.length() + 1, "%s-%s-%sT%s%%3A%s%%3A%s.%03u000%%2B0000",
		STRDATETIME.substr(0, 4).c_str(), STRDATETIME.substr(4, 2).c_str(), STRDATETIME.substr(6, 2).c_str(),
		STRDATETIME.substr(8, 2).c_str(), STRDATETIME.substr(10, 2).c_str(), STRDATETIME.substr(12, 2).c_str(), tb.millitm);

	string_replace_all(strRequestURL, strUrlTime, "XXXX-XX-XXTXX%3AXX%3AXX.XXXXXX%2B0000");

	return result;
}
int get_response_handler(string &strResponseData)
{
	int result = 0;

	size_t stSize = strResponseData.length();
	for (size_t stIdx = 0; stIdx < stSize; stIdx++)
	{
		if (!strResponseData.at(stIdx))
		{
			strResponseData.at(stIdx) = '\x20';
		}
	}
	//printf("utf8 json:[%s]\n", strResponseData.c_str());
	printf("ansi json:[%s]\n", UnicodeToANSI(UTF8ToUnicode(strResponseData)).c_str());

	return result;
}
int post_request_handler(string &strRequestURL, string &strHeaderData, string &strPostFields)
{
	int result = 0;
	struct timeb tb = { 0 };
	std::string strTime = "0000000000000000000";
	std::string strRandom = "============================";
	
	ftime(&tb);
	srand(tb.time);
	
	_snprintf((char *)strTime.c_str(), strTime.length() + 1, "%10lld%03u%03d%03d", tb.time, tb.millitm, random(1000), random(1000));
	
	for (int x = 0; x < strRandom.length() - 1; x++)
	{
		strRandom[x] = pChars[random(64)];
	}
	string_replace_all(strHeaderData, strTime, "0000000000000000000");
	string_replace_all(strHeaderData, strRandom, "============================");

	return result;
}
int post_response_handler(string &strResponseData)
{
	int result = 0;
	
	size_t stSize = strResponseData.length();
	for (size_t stIdx = 0; stIdx < stSize; stIdx++)
	{
		if (!strResponseData.at(stIdx))
		{
			strResponseData.at(stIdx) = '\x20';
		}
	}
	//printf("utf8 json:[%s]\n", strJson.c_str());
	printf("ansi json:[%s]\n", UnicodeToANSI(UTF8ToUnicode(strResponseData)).c_str());

	return result;
}
void visit_get(CURL * pCurl, std::string strIdx = "", size_t stDelayTime = 0, PREQUEST_HANDLER request_handler = 0, PRESPONSE_HANDLER response_handler = 0)
{
	std::string strJson = "";
	std::string strRequestUrl = read_file(TToA(GetProgramPath()) + SCRIPT_FILEPATH + strIdx + "-get-"URL_FILENAME".txt");
	std::string strPostFields = read_file(TToA(GetProgramPath()) + SCRIPT_FILEPATH + strIdx + "-get-"POSTFIELDS_FILENAME".txt");
	std::string strHeaderData = read_file(TToA(GetProgramPath()) + SCRIPT_FILEPATH + strIdx + "-get-"HEADERDATA_FILENAME".txt");
	if ( strRequestUrl.length() <= 0)
	{
		return;
	}
	
	Sleep(stDelayTime);

	if (request_handler)
	{
		request_handler(strRequestUrl, strHeaderData, strPostFields);
	}

	curl_http_get_data(pCurl, strJson, strRequestUrl, strHeaderData, strPostFields);

	if (response_handler)
	{
		response_handler(strJson);
	}
}

void visit_post(CURL * pCurl, std::string strIdx, size_t stDelayTime = 0, PREQUEST_HANDLER request_handler = 0, PRESPONSE_HANDLER response_handler = 0)
{
	std::string strJson = "";
	std::string strRequestUrl = read_file(TToA(GetProgramPath()) + SCRIPT_FILEPATH + strIdx + "-post-"URL_FILENAME".txt");
	std::string strPostFields = read_file(TToA(GetProgramPath()) + SCRIPT_FILEPATH + strIdx + "-post-"POSTFIELDS_FILENAME".txt");
	std::string strHeaderData = read_file(TToA(GetProgramPath()) + SCRIPT_FILEPATH + strIdx + "-post-"HEADERDATA_FILENAME".txt");
	
	if (strRequestUrl.length() <= 0)
	{
		return;
	}
	
	Sleep(stDelayTime);
	
	if (request_handler)
	{
		request_handler(strRequestUrl, strHeaderData, strPostFields);
	}
	
	curl_http_post_data(pCurl, strJson, strRequestUrl, strHeaderData, UnicodeToUTF8(ANSIToUnicode(strPostFields)).c_str());
	
	if (response_handler)
	{
		response_handler(strJson);
	}
}

int main(int argc, char ** argv)
{
	int result = 0;
	CURL * pCurl = 0;
	std::string strCounter = read_file(TToA(GetProgramPath()) + SCRIPT_COUNTER);
	std::string strDelayTime = read_file(TToA(GetProgramPath()) + SCRIPT_DELAYTIME);
	
	init_curl_env(&pCurl);
	
	if (strCounter.length() > 0 && strCounter.length() < 10)
	{
		size_t stDelayTime = 0;
		size_t stCounter = strtoul(strCounter.c_str(), 0, 10);
		
		if (strDelayTime.length() > 0 && strDelayTime.length() < 10)
		{
			stDelayTime = strtoul(strDelayTime.c_str(), 0, 10);
		}
		for (size_t stIdx = 0; stIdx < stCounter; stIdx++)
		{
			char czIdx[MAXCHAR] = { 0 };
			_snprintf(czIdx, sizeof(czIdx) / sizeof(*czIdx), "%ld", stIdx);

			if (IsFileExists(std::string(TToA(GetProgramPath()) + SCRIPT_FILEPATH + czIdx + "-get-"URL_FILENAME".txt").c_str()))
			{
				printf("\n====================================[get-%s]====================================\n", czIdx);
				visit_get(pCurl, czIdx, stDelayTime, get_request_handler, get_response_handler);
			}
			if (IsFileExists(std::string(TToA(GetProgramPath()) + SCRIPT_FILEPATH + czIdx + "-post-"URL_FILENAME".txt").c_str()))
			{
				printf("\n====================================[post-%s]====================================\n", czIdx);
				visit_post(pCurl, czIdx, stDelayTime, post_request_handler, post_response_handler);
			}
		}
	}
	exit_curl_env(&pCurl);

	getchar();

	return 0;
}

