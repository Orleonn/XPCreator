#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif



#define XPCREATOR_VERSION L"0.1.0"



inline std::wstring XPSprintf(const wchar_t* format, ...)
{
	wchar_t buf[2048]{};
	va_list args;
	va_start(args, format);
	std::vswprintf(buf, format, args);
	va_end(args);
	return { buf };
}

inline std::wstring XPGetBinaryDir()
{
	wchar_t path[520]{};
#pragma warning(push)
#pragma warning(disable : 6386)
	GetModuleFileNameW(nullptr, path, sizeof(path));
#pragma warning(pop)
	*wcsrchr(path, L'\\') = L'\0';
	return { path };
}
