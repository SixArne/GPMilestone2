#pragma once
#include <windows.h>
#include <string>
#include <tchar.h>

void SaveDataToClipboard(const std::string& data)
{
	// open the clipboard
	if (!OpenClipboard(NULL)) {
		return;
	}

	// convert the ASCII string to Unicode text format
	int length = MultiByteToWideChar(CP_UTF8, 0, data.c_str(), -1, NULL, 0);
	WCHAR* unicodeData = new WCHAR[length];
	MultiByteToWideChar(CP_UTF8, 0, data.c_str(), -1, unicodeData, length);

	// allocate global memory for the Unicode text
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR) * length);

	// copy the Unicode text to the allocated memory
	WCHAR* pMem = (WCHAR*)GlobalLock(hMem);
	memcpy(pMem, unicodeData, sizeof(WCHAR) * length);
	GlobalUnlock(hMem);

	delete[] unicodeData;

	// save the memory handle to the clipboard
	SetClipboardData(CF_UNICODETEXT, hMem);

	// close the clipboard
	CloseClipboard();
}

//
//auto toSave = std::format("{}, {}, {}", pos.x, pos.y, pos.z);
//SaveDataToClipboard(toSave.c_str());