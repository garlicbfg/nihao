#include "LayoutStructure.h"




void LayoutStructure::addElement(const std::string& id, const std::string& parentId, const LPCTSTR lpClassName, const LPCTSTR lpWindowName, const DWORD dwStyle, LayoutFunc lf, HwndLayoutFunc hlf, InitElementFunc ief, int bindNum)
{
	WinElement& elem = (elements[id] = { id, parentId, lpClassName, lpWindowName, dwStyle, lf, hlf, ief, bindNum });

	if (bindNum > maxBindNum)
		maxBindNum = bindNum;

	if (parentId.size() != 0)
		elements.at(parentId).childs.push_back(&elem);
	else
		root = &elem;
}



void LayoutStructure::addLayoutFunc(const std::string& id, LayoutFunc layoutFunc)
{
	elements.at(id).layoutFunc = layoutFunc;
}



void LayoutStructure::addHwndLayoutFunc(const std::string& id, HwndLayoutFunc hwndLayoutFunc)
{
	elements.at(id).hwndLayoutFunc = hwndLayoutFunc;
}



void LayoutStructure::addInitElementFunc(const std::string& id, InitElementFunc initElementFunc)
{
	elements.at(id).initElementFunc = initElementFunc;
}



void LayoutStructure::setBindNum(const std::string& id, const int bindNum)
{
	elements.at(id).bindNum = bindNum;

	if (bindNum > maxBindNum)
		maxBindNum = bindNum;
}



HWND LayoutStructure::getHwnd(const std::string& id)
{
	return elements.at(id).hwnd;
}



HWND LayoutStructure::getHwnd(const int num)
{
	return bindNumTable[num]->hwnd;
}



void LayoutStructure::buildWindow(HINSTANCE hInstance, HWND hwnd)
{
	bindNumTable.resize(maxBindNum + 1, nullptr);
	createElement(hInstance, hwnd, root);
}



void LayoutStructure::show(const WinElement* node)
{
	for (const WinElement* n : node->childs)
		show(n);

	if (node->hwnd > 0)
		ShowWindow(node->hwnd, SW_SHOW);
}



void LayoutStructure::hide(const int num)
{
	hide(bindNumTable[num]);
}



void LayoutStructure::resize(const int num, const WinRect& winRect)
{
	resize(bindNumTable[num], winRect);
}



void LayoutStructure::show(const int num)
{
	show(bindNumTable[num]);
}



void LayoutStructure::hide(const WinElement* node)
{
	for (const WinElement* n : node->childs)
		hide(n);

	if (node->hwnd > 0)
		ShowWindow(node->hwnd, SW_HIDE);
}



void LayoutStructure::resize(const WinElement* node, const WinRect& winRect)
{
	WinRect forChild;

	if (node->hwnd != nullptr)
	{
		MoveWindow(node->hwnd, winRect.x, winRect.y, winRect.w, winRect.h, true);
		forChild = node->hwndLayoutFunc(node->hwnd);
	}
	else
	{
		forChild = winRect;
	}

	for (WinElement* n : node->childs)
	{
		WinRect c = n->layoutFunc(forChild.w, forChild.h);
		c.x += forChild.x;
		c.y += forChild.y;
		resize(n, c);
	}
}



void LayoutStructure::createElement(HINSTANCE hInstance, HWND parentHwnd, WinElement* we)
{
	HWND hwnd = (we->lpClassName == nullptr) ? nullptr :
		CreateWindow(we->lpClassName, we->lpWindowName, WS_CHILD | we->dwStyle, 0, 0, 0, 0, parentHwnd, NULL, hInstance, NULL);
	
	we->hwnd = hwnd;

	if (we->bindNum >= 0)
		bindNumTable[we->bindNum] = we;

	if(we->layoutFunc == nullptr)
		we->layoutFunc = defaultLayoutFunc;

	if (we->hwndLayoutFunc == nullptr)
		we->hwndLayoutFunc = defaultHwndFunc;

	if (we->initElementFunc != nullptr)
		we->initElementFunc(hwnd);

	for (int i = 0; i < we->childs.size(); i++)
		createElement(hInstance, parentHwnd, we->childs[i]);
}

