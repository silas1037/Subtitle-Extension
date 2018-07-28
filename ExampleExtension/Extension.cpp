#include "stdafx.h"

extern "C"
{
	/**
	* Param sentence: wstring of the sentence received by NextHooker
	* Param info: map containing additional info about the sentence
	* You can modify the params, and future extensions will receive them
	* NextHooker will display the sentence after all extensions have had a chance to process and/or modify it
	* THIS FUNCTION MAY BE RUN SEVERAL TIMES CONCURRENTLY: PLEASE ENSURE THAT IT IS THREAD SAFE!
	*/
	void __declspec(dllexport) OnNewSentence(std::wstring& sentence, std::unordered_map<std::string, int>& info)
	{
		// This example extension does nothing except place sentences from the hook currently selected by the user into the clipboard
		if (info["current select"])
		{
			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (sentence.size() + 1) * sizeof(wchar_t));
			memcpy(GlobalLock(hMem), sentence.c_str(), (sentence.size() + 1) * sizeof(wchar_t));
			GlobalUnlock(hMem);
			OpenClipboard(0);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hMem);
			CloseClipboard();
		}
	}
}
