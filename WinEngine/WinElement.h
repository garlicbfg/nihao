#pragma once
#include <string>
#include <vector>
#include <functional>
#include <Windows.h>
#include "WinRect.h"

using LayoutFunc = std::function<WinRect(const int width, const int height)>;
using HwndLayoutFunc = std::function<WinRect(HWND hwnd)>;
using InitElementFunc = std::function<void(HWND hwnd)>;

struct WinElement
{
	std::string id;
	std::string parentId;
	LPCTSTR lpClassName;
	LPCTSTR lpWindowName;
	DWORD dwStyle;
	LayoutFunc layoutFunc;
	HwndLayoutFunc hwndLayoutFunc;
	InitElementFunc initElementFunc;
	int bindNum;
	std::vector<WinElement*> childs;
	HWND hwnd;
};
