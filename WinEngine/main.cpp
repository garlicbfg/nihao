#include "stdafx.h"
#include "resource.h"
#include "WindowManager.h"





int __stdcall  runSample(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WindowManager wm;

	wm.windowTitle = "TC1";
	wm.windowClassName = "winengine";

	wm.IDC_WE = IDC_WIN32PROJECT1;
	wm.IDI_SMALL_WE = IDI_SMALL;
	wm.IDI_WE = IDI_WIN32PROJECT1;
	//---------------------------------------------------------
	wm.addElement("tabctrl1", "", WC_TABCONTROL, "", WS_CLIPSIBLINGS | WS_VISIBLE);
	wm.addLayout("layout1", "tabctrl1");
	wm.addLayout("layout1-1", "layout1");
	wm.addLayout("layout1-2", "layout1");
	wm.addElement("btn1", "layout1-1", WC_BUTTON, "Button 1", 0);
	wm.addElement("btn2", "layout1-1", WC_BUTTON, "Button 2", 0);
	wm.addElement("btn3", "layout1-1", WC_BUTTON, "Button 3", 0);
	wm.addElement("btn4", "layout1-1", WC_BUTTON, "Button 4", 0);
	wm.addElement("btn5", "layout1-2", WC_BUTTON, "Button 5", 0);
	wm.addElement("btn6", "tabctrl1", WC_BUTTON, "Button 6", 0);
	wm.addElement("static1", "tabctrl1", WC_STATIC, "=== Text ===\nLine 1\nLine 2\n=== End text ===", WS_BORDER);
	wm.addElement("edit1", "tabctrl1", WC_EDIT, "", 0);

	// add layout for control tab
	wm.addHwndLayoutFunc(
		"tabctrl1",
		[](HWND hwnd)
		{	
			RECT rc;
			GetClientRect(hwnd, &rc);
			TabCtrl_AdjustRect(hwnd, FALSE, &rc);
			return WinRect{ rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };
		}
	);

	wm.addLayoutFunc(
		"layout1-1",
		[](const int width, const int height) { return WinRect{ 0, 0, width / 2, height }; }
	);

	wm.addLayoutFunc(
		"layout1-2",
		[](const int width, const int height) { return WinRect{ width / 2, 0, width / 2, height }; }
	);

	wm.addLayoutFunc(
		"btn1",
		[](const int width, const int height) { return WinRect{ 0, 0, width / 2, height / 2 }; }
		);

	wm.addLayoutFunc(
		"btn2",
		[](const int width, const int height) { return WinRect{ width / 2, 0, width / 2, height / 2 }; }
		);

	wm.addLayoutFunc(
		"btn3",
		[](const int width, const int height) { return WinRect{ 0, height / 2, width / 2, height / 2 }; }
		);

	wm.addLayoutFunc(
		"btn4",
		[](const int width, const int height) { return WinRect{ width / 2, height / 2, width / 2, height / 2 }; }
		);

	wm.addLayoutFunc(
		"btn5",
		[](const int width, const int height) { return WinRect{ 15, 15, width - 30, height - 30 }; }
		);

	wm.addLayoutFunc(
		"btn6",
		[](const int width, const int height) {	return WinRect{ 0, 0, width, height }; }
		);

	wm.addLayoutFunc(
		"static1",
		[](const int width, const int height) { return WinRect{ 10, 10, width - 20, height - 20 }; }
		);

	wm.addLayoutFunc(
		"edit1",
		[](const int width, const int height) { return WinRect{ width / 2 - 50, height / 2 - 15, 100, 30 }; }
		);

	wm.addInitElementFunc(
		"tabctrl1",
		[](HWND hwnd)
		{
			TC_ITEM tabitem;
			tabitem.mask = TCIF_TEXT;
			tabitem.iImage = 0;
			tabitem.pszText = "Tab 1";
			SendMessage(hwnd, TCM_INSERTITEM, 0, LPARAM(&tabitem));
			tabitem.iImage = 1;
			tabitem.pszText = "Tab 2";
			SendMessage(hwnd, TCM_INSERTITEM, 1, LPARAM(&tabitem));
			tabitem.iImage = 2;
			tabitem.pszText = "Tab 3";
			SendMessage(hwnd, TCM_INSERTITEM, 2, LPARAM(&tabitem));
			tabitem.iImage = 3;
			tabitem.pszText = "Tab 4";
			SendMessage(hwnd, TCM_INSERTITEM, 3, LPARAM(&tabitem));
		}
	);

	wm.setOnCreateFunc(
		[&](HWND hwnd)
		{
			for (int i = 1; i <= 4; i++)
				WindowManager::hide(i);

			//HWND hwndTab = elementsTable.at("tabctrl1");
			HWND hwndTab = WindowManager::getHwnd(0);

			NMHDR nmhdr;
			nmhdr.code = TCN_SELCHANGE;
			nmhdr.idFrom = 0;
			nmhdr.hwndFrom = 0;
			SendMessage(hwndTab, WM_NOTIFY, 0, (LPARAM)&nmhdr);

			RECT r1;
			GetClientRect(hwnd, &r1);
			WindowManager::resize(0, { r1.left, r1.top, r1.right - r1.left, r1.bottom - r1.top });
		}
	);

	wm.setBindNum("tabctrl1", 0);
	wm.setBindNum("layout1", 1);
	wm.setBindNum("btn6", 2);
	wm.setBindNum("static1", 3);
	wm.setBindNum("edit1", 4);

	wm.addMessageHandlerFunc(
		WM_NOTIFY,
		[&](HWND hwnd, WPARAM wParam, LPARAM lParam)
		{
			if (((LPNMHDR)lParam)->code == TCN_SELCHANGE)
			{
				int wmId = SendMessage(WindowManager::getHwnd(0), TCM_GETCURSEL, 0, 0);
				for (int i = 1; i <= 4; i++)
					WindowManager::hide(i);

				WindowManager::show(wmId + 1);

				//WindowManager::layoutStructure.resizeVisible(wmId + 1, );
				return true;
			}

			return false;
		}
	);

	wm.addMessageHandlerFunc(
		WM_COMMAND,
		[&](HWND hwnd, WPARAM wParam, LPARAM lParam)
		{
			int wmId = LOWORD(wParam);
			int wmEvent = HIWORD(wParam);
			HWND hwndTab;
			switch (wmId)
			{
			case IDM_ABOUT:
				//hwndTab = WindowManager::getElementsHwnd("tabctrl1");
				hwndTab = wm.getHwnd(0);
				SendMessage(hwndTab, TCM_SETCURFOCUS, 2, 0);
				return true;
			case IDM_EXIT:
				DestroyWindow(hwnd);
				return true;
			default:
				return false;
			}
		}
	);
	
	// Parse the menu selections:
	return wm.run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}



int __stdcall  _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	runSample(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}



