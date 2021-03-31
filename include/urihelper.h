#ifndef URIHELPER__
#define URIHELPER__

#include <cctype>
#include "str.h"

/* Converts an integer value to its hex character*/
inline char to_hex(char code) { return "0123456789abcdef"[code & 15]; }

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char* url_encode(char* str) {
	char *pstr = str, *buf = new char[cmplx::strlen(str) * 3 + 1], *pbuf = buf;
	while(*pstr) {
		if(isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
			*pbuf++ = *pstr;
		else
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}

Str uriComponent(const char* param, const char* value) {
	char* val = url_encode((char*)value);
	Str res = strn(param + "=" + val);
	delete[] val;
	return res;
}

#endif