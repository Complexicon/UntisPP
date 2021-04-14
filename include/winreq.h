#ifndef WINREQ__
#define WINREQ__

#include <Windows.h>
#include <WinHttp.h>
#include "str.h"
#include <string>

using namespace std; // bad practice
typedef const char* cstr;

wstring get_utf16(cstr str, int codepage) {
	int sz = MultiByteToWideChar(codepage, 0, &str[0], cmplx::strlen(str), 0, 0);
	wstring res(sz, 0);
	MultiByteToWideChar(codepage, 0, &str[0], cmplx::strlen(str), &res[0], sz);
	return res;
}

Str HttpsWebRequestPost(cstr domain, cstr url, cstr data, cstr addHdr = "", cstr content = "application/json") {
	// Extra
	DWORD data_len = cmplx::strlen(data);

	wstring sdomain = get_utf16(domain, CP_UTF8);
	wstring surl = get_utf16(url, CP_UTF8);
	wstring header = get_utf16(str("Content-Type: " + content + "\r\n" + addHdr + "\r\n"), CP_UTF8); // bad practice fix soon!
	Str response;

	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	char* pszOutBuffer;
	bool bResults = false;
	HINTERNET hSession = 0, hConnect = 0, hRequest = 0;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(L"UntisPP/1.2", 0, 0, 0, 0);

	// Specify an HTTP server.
	if(hSession)
		hConnect = WinHttpConnect(hSession, sdomain.c_str(), INTERNET_DEFAULT_PORT, 0);

	// Create an HTTP request handle.
	if(hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"POST", surl.c_str(), 0, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
									  WINHTTP_FLAG_SECURE);

	if(WinHttpSendRequest(hRequest, header.c_str(), -1, (void*)data, data_len, data_len, 0) &&
	   WinHttpReceiveResponse(hRequest, 0)) {
		DWORD dwStatusCode = 0;
		DWORD dwSize = sizeof(dwStatusCode);

		WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
							&dwStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);
		do {
			// Check for available data.
			dwSize = 0;
			if(!WinHttpQueryDataAvailable(hRequest, &dwSize))
				printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());

			// Allocate space for the buffer.
			pszOutBuffer = new char[dwSize + 1];
			if(!pszOutBuffer) {
				printf("Out of memory\n");
			} else {
				ZeroMemory(pszOutBuffer, dwSize + 1);

				if(!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				else
					response += pszOutBuffer;
				// Free the memory allocated to the buffer.
				delete[] pszOutBuffer;
			}
		} while(dwSize > 0);
	} else
		printf("[WinReq] Error %u has occurred.\n", GetLastError());

	// Close any open handles.
	if(hRequest)
		WinHttpCloseHandle(hRequest);
	if(hConnect)
		WinHttpCloseHandle(hConnect);
	if(hSession)
		WinHttpCloseHandle(hSession);

	return response;
}

#endif