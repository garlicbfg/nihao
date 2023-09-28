#include "WindowManager.h"


HINSTANCE WindowManager::hInstance;
LayoutStructure WindowManager::layoutStructure;
WMHandler WindowManager::wmHandler;

BOOL registerClass(WNDPROC, LPCTSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);




int __stdcall  WindowManager::run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hwnd;

	WindowManager::hInstance = hInstance;

	if (!registerClass(&WindowManager::WndProc, windowClassName.c_str(), COLOR_WINDOW))
		return FALSE;

	if (!(hwnd = CreateWindow(windowClassName.c_str(), windowTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 50, 640, 480, 0, 0, hInstance, NULL)))
		return FALSE;

	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}



void WindowManager::addElement(const std::string& id, const std::string& parentId, const LPCTSTR lpClassName,
	const LPCTSTR lpWindowName, const DWORD dwStyle, LayoutFunc lf, HwndLayoutFunc hlf, InitElementFunc ief,
	int bindNum)
{
	layoutStructure.addElement(id, parentId, lpClassName, lpWindowName, dwStyle, lf, hlf, ief, bindNum);
}



void WindowManager::addLayout(const std::string& id, const std::string& parentId, LayoutFunc lf,
	HwndLayoutFunc hlf, InitElementFunc ief, int bindNum)
{
	layoutStructure.addElement(id, parentId, nullptr, "", 0, lf, hlf, ief, bindNum);
}



void WindowManager::addLayoutFunc(const std::string& id, LayoutFunc layoutFunc)
{
	layoutStructure.addLayoutFunc(id,layoutFunc);
}



void WindowManager::addHwndLayoutFunc(const std::string& id, HwndLayoutFunc hwndLayoutFunc)
{
	layoutStructure.addHwndLayoutFunc(id, hwndLayoutFunc);
}



void WindowManager::addInitElementFunc(const std::string& id, InitElementFunc initElementFunc)
{
	layoutStructure.addInitElementFunc(id, initElementFunc);
}



void WindowManager::setBindNum(const std::string& id, const int bindNum)
{
	layoutStructure.setBindNum(id, bindNum);
}



void WindowManager::addMessageHandlerFunc(UINT msg, WmFunc f)
{
	switch (msg)
	{
	case WM_CREATE:
		throw __FUNCTION__;

	case WM_DESTROY:
		throw __FUNCTION__;

	case WM_NOTIFY:
		wmHandler.addOnWmNotify(f);
		break;
	
	case WM_SIZE:
		throw __FUNCTION__;

	case WM_PAINT:
		throw __FUNCTION__;

	case WM_COMMAND:
		wmHandler.addOnWmCommand(f);
		break;

	default:
		throw __FUNCTION__;
	}
}



void WindowManager::setOnCreateFunc(OnCreateFunc f)
{
	wmHandler.setOnCreateFunc(f);
}



void WindowManager::show(const int num)
{
	layoutStructure.show(num);
}



void WindowManager::hide(const int num)
{
	layoutStructure.hide(num);
}



void WindowManager::resize(const int num, const WinRect& winRect)
{
	layoutStructure.resize(num, winRect);
}



HWND WindowManager::getHwnd(const std::string& id)
{
	return layoutStructure.getHwnd(id);
}



HWND WindowManager::getHwnd(const int num)
{
	return layoutStructure.getHwnd(num);
}

// private


BOOL WindowManager::registerClass(WNDPROC Proc, LPCTSTR szName, UINT brBackground)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WE));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(brBackground + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_WE);
	wcex.lpszClassName = szName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL_WE));

	return (RegisterClassEx(&wcex) != 0);
}


void DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap)
{
	HBITMAP hbm, hOldbm;
	HDC hMemDC;
	BITMAP bm;

	hMemDC = CreateCompatibleDC(hDC);
	// Select the new bitmap
	hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmap);

	// Copy the bits from the memory DC into the current dc
	BitBlt(hDC, x, y, 450, 400, hMemDC, 0, 0, SRCCOPY);

	// Restore the old bitmap
	SelectObject(hMemDC, hOldbm);

	DeleteDC(hMemDC);
}



LRESULT CALLBACK WindowManager::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
		case WM_CREATE:
			buildWindow(hwnd);
			wmHandler.handleOnCreateFunc(hwnd);
			return 0;

		case WM_NOTIFY:
			if (wmHandler.handleWmNotify(hwnd, wParam, lParam))
				return 0;
			else
				return DefWindowProc(hwnd, msg, wParam, lParam);

		case WM_SIZE:
		{
			int nWidth = LOWORD(lParam);
			int nHeight = HIWORD(lParam);
			MoveWindow(getHwnd(0), 0, 0, nWidth, nHeight, TRUE);
			WindowManager::resize(0, { 0, 0, nWidth, nHeight });
			break;
		}

		case WM_COMMAND:
			if (wmHandler.handleWmCommand(hwnd, wParam, lParam))
				return 0;
			else
				return DefWindowProc(hwnd, msg, wParam, lParam);

		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);

			//HBITMAP hBitmap = (HBITMAP)LoadImage(hInstance, "map.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

			//DrawBitmap(hdc, 100, 100, hBitmap);

			EndPaint(hwnd, &ps);

			//DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}



void WindowManager::buildWindow(HWND hwnd)
{
	layoutStructure.buildWindow(hInstance, hwnd);
}
/*
void WindowManager::buildWindow(HWND hwnd)
{
	hwndTable.resize(windowStructure.getMaxBindNum() + 1, 0);

	std::map<int, int> bindPosition;
	createElement(hwnd, windowStructure.getRootId(), -1, bindPosition);

	layoutStructure.sortNodes(bindPosition);
}



void WindowManager::createElement(HWND mainWinHwnd, const std::string& id, const int parentPosition, std::map<int, int>& bindPosition)
{
	const WinElement* we = windowStructure.getElement(id);

	HWND hwnd = (we->lpClassName == nullptr) ? nullptr :
		CreateWindow(we->lpClassName, we->lpWindowName, WS_CHILD | we->dwStyle, 0, 0, 0, 0, mainWinHwnd, NULL, hInstance, NULL);

	elementsTable[id] = hwnd;

	if (we->bindNum >= 0)
		hwndTable[we->bindNum] = hwnd;

	if (we->initElementFunc != nullptr)
		we->initElementFunc(hwnd);

	int position = parentPosition < 0 ? layoutStructure.addWithoutParent(we->layoutFunc, hwnd, we->hwndLayoutFunc) :
		layoutStructure.add(parentPosition, we->layoutFunc, hwnd, we->hwndLayoutFunc);

	if (we->bindNum >= 0)
		bindPosition[we->bindNum] = position;

	for (int i = 0; i < we->childs.size(); i++)
		createElement(mainWinHwnd, we->childs[i], position, bindPosition);
}*/