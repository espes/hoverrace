
// Str.h
// String support functions.
//
// Copyright (c) 2009 Michael Imamura.
//
// Licensed under GrokkSoft HoverRace SourceCode License v1.0(the "License");
// you may not use this file except in compliance with the License.
//
// A copy of the license should have been attached to the package from which
// you have taken this file. If you can not find the license you can not use
// this file.
//
//
// The author makes no representations about the suitability of
// this software for any purpose.  It is provided "as is" "AS IS",
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
//
// See the License for the specific language governing permissions
// and limitations under the License.

#include "StdAfx.h"

#ifndef _WIN32
#	include <iconv.h>
#endif

#include "Str.h"

using namespace HoverRace::Util;

/**
 * Convert a UTF-8 string to a wide string.
 * The caller must use OS::Free() on the result.
 * @param s The UTF-8-encoded string (may not be NULL).
 * @return A wide string (must be freed by the caller) (never NULL).
 */
wchar_t *Str::Utf8ToWide(const char *s)
{
	ASSERT(s != NULL);
	
	size_t sz = strlen(s) + 1;
	wchar_t *retv = (wchar_t*)malloc(sz * sizeof(wchar_t));
	if (sz == 1) {
		*retv = L'\0';
		return retv;
	}

#	ifdef _WIN32
		int ct = MultiByteToWideChar(CP_UTF8, 0, s, -1, retv, sz);
		while (ct == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				sz *= 2;
				retv = (wchar_t*)realloc(retv, sz * sizeof(wchar_t));
				ct = MultiByteToWideChar(CP_UTF8, 0, s, -1, retv, sz);
			}
			else {
				ASSERT(FALSE);
				*retv = L'\0';
				break;
			}
		}

		return retv;
#	else
		//TODO: Initialize icv only once per thread.
		iconv_t icv = iconv_open("WCHAR_T", "UTF-8");
		size_t origsz = sz - 1;
		for (;;) {
			char *inbuf = const_cast<char*>(s);
			size_t insz = origsz;
			char *outbuf = reinterpret_cast<char*>(retv);
			size_t outsz = sz - 1;
			size_t ct = iconv(icv, &inbuf, &insz, &outbuf, &outsz);
			if (ct == (size_t)-1) {
				switch (errno) {
					case EILSEQ:
						free(retv);
						iconv_close(icv);
                        return NULL;
						//return wcsdup(L"#<Invalid UTF-8 sequence>");
					case EINVAL:
						free(retv);
						iconv_close(icv);
                        return NULL;
						//return wcsdup(L"#<Incomplete UTF-8 sequence>");
					case E2BIG:
						sz *= 2;
						retv = (wchar_t*)realloc(retv, sz * sizeof(wchar_t));
				}
			}
			else {
				*((wchar_t*)outbuf) = L'\0';
				break;
			}
		}
		iconv_close(icv);
		return retv;
#	endif
}

/**
 * Convert a wide string to a UTF-8 string.
 * The caller must use OS::Free() on the result.
 * @param ws The wide string (may not be NULL).
 * @return A UTF-8-encoded string (must be freed by the caller) (never NULL).
 */
char *Str::WideToUtf8(const wchar_t *ws)
{
	ASSERT(ws != NULL);

#	ifdef _WIN32
		if (*ws == '\0') {
			char *retv = (char*)malloc(1);
			*retv = '\0';
			return retv;
		}

		size_t sz = wcslen(ws) * 3 + 1;  // Initial guess.
		char *retv = (char*)malloc(sz);
		int ct = WideCharToMultiByte(CP_UTF8, 0, ws, -1, retv, sz, NULL, NULL);
		while (ct == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				sz *= 2;
				retv = (char*)realloc(retv, sz);
				ct = WideCharToMultiByte(CP_UTF8, 0, ws, -1, retv, sz, NULL, NULL);
			}
			else {
				ASSERT(FALSE);
				*retv = '\0';
				break;
			}
		}

		return retv;
#	else
		//TODO: Use iconv.
		throw std::exception();
#	endif
}
