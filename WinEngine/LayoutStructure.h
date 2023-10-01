#pragma once
#include <map>
#include "WinElement.h"



// stores windows elements, sorted by string id and by number id;
class LayoutStructure
{
public:
	void addElement(const std::string& id, const std::string& parentId, const LPCTSTR lpClassName,
		const LPCTSTR lpWindowName, const DWORD dwStyle, LayoutFunc lf, HwndLayoutFunc hlf, InitElementFunc ief,
		int bindNum);
	void addLayoutFunc(const std::string& id, LayoutFunc layoutFunc);
	void addHwndLayoutFunc(const std::string& id, HwndLayoutFunc hwndLayoutFunc);
	void addInitElementFunc(const std::string& id, InitElementFunc initElementFunc);
	void setBindNum(const std::string& id, const int bindNum);
	HWND getHwnd(const std::string& id);
	HWND getHwnd(const int num);
	void buildWindow(HINSTANCE hInstance, HWND hwnd);
	void show(const int num);
	void hide(const int num);
	void resize(const int num, const WinRect& winRect);


private:
	void show(const WinElement* node);
	void hide(const WinElement* node);
	void resize(const WinElement* node, const WinRect& winRect);
	void createElement(HINSTANCE hInstance, HWND mainWinHwnd, WinElement* we);


private:
	int maxBindNum;
	WinElement* root;
	std::map<std::string, WinElement> elements;
	std::vector<WinElement*> bindNumTable;

	LayoutFunc defaultLayoutFunc = [](const int width, const int height) { return WinRect{ 0, 0, width, height }; };
	HwndLayoutFunc defaultHwndFunc = [](HWND hwnd)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);
		return WinRect{ rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };
	};
};
