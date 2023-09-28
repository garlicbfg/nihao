# pragma once
#include "stdafx.h"
#include "LayoutStructure.h"
#include "WMHandler.h"
#include <commctrl.h>




class WindowManager
{
public:
	int __stdcall run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
	void addElement(const std::string& id, const std::string& parentId, const LPCTSTR lpClassName,
		const LPCTSTR lpWindowName,	const DWORD dwStyle, LayoutFunc lf = nullptr, HwndLayoutFunc hlf = nullptr,
		InitElementFunc ief = nullptr, int bindNum = -1);
	void addLayout(const std::string&, const std::string& parentId, LayoutFunc lf = nullptr, HwndLayoutFunc hlf = nullptr,
		InitElementFunc ief = nullptr, int bindNum = -1);
	void addLayoutFunc(const std::string& id, LayoutFunc layoutFunc);
	void addHwndLayoutFunc(const std::string& id, HwndLayoutFunc hwndLayoutFunc);
	void addInitElementFunc(const std::string& id, InitElementFunc initElementFunc);
	void setBindNum(const std::string& id, const int bindNum);
	void addMessageHandlerFunc(UINT msg, WmFunc f);
	void setOnCreateFunc(OnCreateFunc f);
	static void show(const int num);
	static void hide(const int num);
	static void resize(const int num, const WinRect& winRect);
	static HWND getHwnd(const std::string& id);
	static HWND getHwnd(const int num);


public:
	std::string windowTitle;
	std::string windowClassName;
	int IDI_WE;
	int IDI_SMALL_WE;
	int IDC_WE;


private:
	BOOL registerClass(WNDPROC Proc, LPCTSTR szName, UINT brBackground);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void buildWindow(HWND hwnd);

private:
	static HINSTANCE hInstance;
	static LayoutStructure layoutStructure;
	static WMHandler wmHandler;
};