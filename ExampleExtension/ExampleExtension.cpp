// ExampleExtension.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define CURRENT_SELECT 0x1000

extern "C"
{
	void __declspec(dllexport) OnSentence(LPCWSTR sentence, DWORD64 info)
	{
		bool sentenceInCurrentlySelectedTextThread = info & CURRENT_SELECT > 0;
		MessageBoxW(nullptr, sentence, L"Example", MB_OK);
	}
}
