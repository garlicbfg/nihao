# pragma once
#include <Windows.h>
#include <functional>
#include <vector>

using WmFunc = std::function<bool(HWND hwnd, WPARAM wParam, LPARAM lParam)>;
using OnCreateFunc = std::function<void(HWND hwnd)>;

class WMHandler
{
public:
	void setOnCreateFunc(OnCreateFunc f);
	void addOnWmNotify(WmFunc f);
	void addOnWmCommand(WmFunc f);

	void handleOnCreateFunc(HWND hwnd);
	bool handleWmCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
	bool handleWmNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:



private:
	std::vector<WmFunc> onWmNotify;
	std::vector<WmFunc> onWmCommand;
	OnCreateFunc onCreateFunc{ nullptr };
};