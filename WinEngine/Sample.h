#pragma once
#include "stdafx.h"
#include "resource.h"
#include <commctrl.h>

#define MAX_LOADSTRING 100

BOOL registerClass(WNDPROC, LPCTSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInstance;

TCHAR windowTitle[MAX_LOADSTRING];					// The title bar text
TCHAR windowClassName[MAX_LOADSTRING];			// the main window class name
TCHAR tab1Name[MAX_LOADSTRING];
TCHAR tab2Name[MAX_LOADSTRING];
TCHAR tab3Name[MAX_LOADSTRING];

int __stdcall  sample(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hwnd;

	LoadString(hInstance, IDS_APP_TITLE, windowTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN_CLASS_NAME, windowClassName, MAX_LOADSTRING);
	LoadString(hInstance, IDS_TAB1_NAME, tab1Name, MAX_LOADSTRING);
	LoadString(hInstance, IDS_TAB2_NAME, tab2Name, MAX_LOADSTRING);
	LoadString(hInstance, IDS_TAB3_NAME, tab3Name, MAX_LOADSTRING);

	hInstance = hInstance;

	if (!registerClass(WndProc, windowClassName, COLOR_WINDOW))
		return FALSE;

	if (!(hwnd = CreateWindow(windowClassName, windowTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 50, 640, 480, 0, 0, hInstance, NULL)))
		return FALSE;

	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}



BOOL registerClass(WNDPROC Proc, LPCTSTR szName, UINT brBackground)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(brBackground + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_WIN32PROJECT1);
	wcex.lpszClassName = szName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return (RegisterClassEx(&wcex) != 0);
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TC_ITEM tabitem;
	static HWND tab[3] = { 0 };

	static HWND hwndTab;

	switch (msg)
	{
		case WM_CREATE:
		{
						  RECT rcClient;
						  GetClientRect(hwnd, &rcClient);
						  hwndTab = CreateWindow(WC_TABCONTROL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, rcClient.right, rcClient.bottom, hwnd, NULL, hInstance, NULL);
						  tabitem.mask = TCIF_TEXT;
						  tabitem.iImage = 0;
						  tabitem.pszText = tab1Name;
						  SendMessage(hwndTab, TCM_INSERTITEM, 0, LPARAM(&tabitem));
						  tabitem.iImage = 1;
						  tabitem.pszText = tab2Name;
						  SendMessage(hwndTab, TCM_INSERTITEM, 1, LPARAM(&tabitem));
						  tabitem.iImage = 2;
						  tabitem.pszText = tab3Name;
						  SendMessage(hwndTab, TCM_INSERTITEM, 2, LPARAM(&tabitem));
						  
						  RECT rc;
						  GetClientRect(hwndTab, &rc);
						  TabCtrl_AdjustRect(hwndTab, FALSE, &rc);

						  tab[0] = CreateWindow(WC_BUTTON, NULL, WS_CHILD | WS_VISIBLE, 30, 30, 40, 68, hwndTab, NULL, hInstance, NULL);
						  tab[1] = CreateWindow(WC_EDIT, NULL, WS_CHILD, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hwnd, NULL, hInstance, NULL);
						  tab[2] = CreateWindow(WC_BUTTON, NULL, WS_CHILD, 90, 90, 40, 68, hwndTab, NULL, hInstance, NULL);


						  HWND hwndStatic = CreateWindow(WC_STATIC, "", WS_CHILD | WS_VISIBLE | WS_BORDER,
							  rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hwndTab, NULL, hInstance, NULL);

						  return 0;
		}

		case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code == TCN_SELCHANGE)
		{
			wmId = SendMessage(hwndTab, TCM_GETCURSEL, 0, 0);

			for (int i = 0; i < 3; i++)
			{
				if (tab[i])
					ShowWindow(tab[i], SW_HIDE);
			}
			ShowWindow(tab[wmId], SW_SHOW);
		}
		break;

		case WM_SIZE:
		{
						INT nWidth = LOWORD(lParam);
						INT nHeight = HIWORD(lParam);
						MoveWindow(hwndTab, 0, 0, nWidth, nHeight, TRUE);
						break;
		}

		case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
			case IDM_ABOUT:
			SendMessage(hwndTab, TCM_SETCURFOCUS, 2, 0);
			break;
			case IDM_EXIT:
			DestroyWindow(hwnd);
			break;
			default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;

		case WM_PAINT:

		hdc = BeginPaint(hwnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hwnd, &ps);
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}