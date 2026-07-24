#include "Encoding.h"
#include <windows.h>

std::string Utf8ToSjis(const std::string& utf8Str)
{
	if (utf8Str.empty()) return "";

	// UTF-8 → UTF-16(ワイド文字)
	int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
	std::wstring wideStr(wideLen, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, wideStr.data(), wideLen);

	// UTF-16 → Shift-JIS(CP932)
	int sjisLen = WideCharToMultiByte(932, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string sjisStr(sjisLen, '\0');
	WideCharToMultiByte(932, 0, wideStr.c_str(), -1, sjisStr.data(), sjisLen, nullptr, nullptr);

	// 末尾の余分な\0を除去
	if (!sjisStr.empty() && sjisStr.back() == '\0') sjisStr.pop_back();

	return sjisStr;
}