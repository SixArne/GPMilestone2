#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <functional>
#include <chrono>

std::string WideStringToUTF8(const std::wstring& wide_string)
{
	int utf8_length = WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string utf8_string(utf8_length, 0);
	WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, &utf8_string[0], utf8_length, nullptr, nullptr);
	return utf8_string;
}

std::vector<std::string> ConvertAnimationNames(const std::vector<std::wstring>& wide_strings)
{
	// Convert std::vector<std::wstring> to std::vector<std::string>
	std::vector<std::string> utf8_strings;
	utf8_strings.reserve(wide_strings.size());
	for (const auto& wide_string : wide_strings) {
		utf8_strings.push_back(WideStringToUTF8(wide_string));
	}

	return utf8_strings;
}