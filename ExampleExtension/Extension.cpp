#include "Extension.h"
#include <stdio.h>
#include <tchar.h>
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//GDI库
#include <GdiPlus.h>
#pragma comment(lib,"GdiPlus.lib")

#include <SDKDDKVer.h>

using namespace Gdiplus;
using namespace DllExports;

//能够使用Visual C＋＋中的一些特殊COM支持类。有了这个头文件，gdiplus.h里面有东西就可以用了。
//#include <comdef.h>

//主窗口COMMAND处理
HMODULE lrc;
typedef void(*CreateLRC)();
typedef void(*InitGDI)();
typedef void(*ReleaseDrawLRC)();
typedef void(*DrawLRC_SHOW_TEXT)(WCHAR *text);
typedef void(*SetLRCColor)(int lrc_color);
typedef void(*SetDrawLRCTime)(LONG time);
typedef void(*LockingLRC)();

DrawLRC_SHOW_TEXT draw_text;
SetLRCColor setcolor;
SetDrawLRCTime settime;
LockingLRC lockingLRC;

#ifdef _UNICODE
#define COUT wcout
#else
#define COUT cout
#endif

std::hash<std::wstring> h;
using namespace std;
void inisub() {
	lrc = LoadLibrary(_T("DrawLRC_DLL.dll"));
	InitGDI in;
	in = (InitGDI)GetProcAddress(lrc, "InitGDI");
	in();
	CreateLRC mb;
	mb = (CreateLRC)GetProcAddress(lrc, "CreateLRC");
	mb();
	draw_text = (DrawLRC_SHOW_TEXT)GetProcAddress(lrc, "DrawLRC_SHOW_TEXT");
	setcolor = (SetLRCColor)GetProcAddress(lrc, "SetLRCColor");
	settime = (SetDrawLRCTime)GetProcAddress(lrc, "SetDrawLRCTime");
	lockingLRC = (LockingLRC)GetProcAddress(lrc, "LockingLRC");
}

void putsub(LPCWSTR Ltext) {
	WCHAR ch[1000] = { '\0' };
	wcscpy_s(ch, Ltext);
	WCHAR * text = ch;
	draw_text(text);
}
static void make_console() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	std::cout << "debug" << std::endl;
}
map <size_t, LPCWSTR> m1;
int mark = 0;
bool loadText()
{

	const std::locale empty_locale = std::locale::empty();
	typedef std::codecvt_utf8<wchar_t> converter_type;  //std::codecvt_utf16
	const converter_type* converter = new converter_type;
	const std::locale utf8_locale = std::locale(empty_locale, converter);

	std::wifstream fin1("jpdic.txt");  //input
	fin1.imbue(utf8_locale);
	std::wifstream fin2("zhdic.txt");  //input
	fin2.imbue(utf8_locale);
	std::wstring line1;
	std::wstring line2;

	int idx = 0;
	while (!fin1.eof() && !fin2.eof())
	{
		std::getline(fin1, line1);

		line1 = line1.append(L"\n");

		//CString str1(line1.c_str());
		std::getline(fin2, line2);
		CString str2(line2.c_str());

		LPCWSTR lpcwStr = str2.AllocSysString();
		m1.insert(pair<size_t, LPCWSTR>(h(line1), lpcwStr));
		//printf("%s\n", (LPCTSTR)str);
		idx++;
	}
	fin1.close();
	fin2.close();

	cout << "Load Texts:0x" << hex << idx << endl;
	return true;
}
BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxW(NULL, L"Extension Loaded", L"Example", MB_OK);
		make_console();
		loadText();
		inisub();
		putsub(L"");
		break;
	case DLL_PROCESS_DETACH:
		MessageBoxW(NULL, L"Extension Removed", L"Example", MB_OK);
		break;
	}
	return TRUE;
}

//#define COPY_CLIPBOARD
//#define EXTRA_NEWLINES

/*
	Param sentence: sentence received by Textractor (UTF-16). Can be modified, Textractor will receive this modification only if true is returned.
	Param sentenceInfo: contains miscellaneous info about the sentence (see README).
	Return value: whether the sentence was modified.
	Textractor will display the sentence after all extensions have had a chance to process and/or modify it.
	The sentence will be destroyed if it is empty or if you call Skip().
	This function may be run concurrently with itself: please make sure it's thread safe.
	It will not be run concurrently with DllMain.
*/


bool ProcessSentence(std::wstring& sentence, SentenceInfo sentenceInfo)
{
	

	setlocale(LC_ALL, "chs");
	
	LPCWSTR resl = m1[h(sentence)];
	if (resl != NULL) {
		wcout << resl << endl;
		putsub(resl);
		//WinExec("PutSub.exe 测试",0);
		///delete[]resultStr;
	}

	
	return true;
#ifdef COPY_CLIPBOARD
	// This example extension automatically copies sentences from the hook currently selected by the user into the clipboard.
	if (sentenceInfo["current select"])
	{
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (sentence.size() + 2) * sizeof(wchar_t));
		memcpy(GlobalLock(hMem), sentence.c_str(), (sentence.size() + 2) * sizeof(wchar_t));
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_UNICODETEXT, hMem);
		CloseClipboard();
	}
	return false;
#endif // COPY_CLIPBOARD

#ifdef EXTRA_NEWLINES
	// This example extension adds extra newlines to all sentences.
	sentence += L"\r\n";
	return true;
#endif // EXTRA_NEWLINES
}
