#pragma once
#include <Windows.h> //CP_UTF8
#include <string>	 //std::string
#include <locale>	 //Converting between strings and wstrings
#include <codecvt>	 //Converting between strings and wstrings

class StrUtil 
{
public:
	static std::wstring s2ws(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	static std::string ws2s(const std::wstring& str)
	{

		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return converter.to_bytes(str);
	}

};