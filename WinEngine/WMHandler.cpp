#include "WMHandler.h"






void WMHandler::setOnCreateFunc(OnCreateFunc f)
{
	onCreateFunc = f;
}



void WMHandler::addOnWmNotify(WmFunc f)
{
	onWmNotify.push_back(f);
}



void WMHandler::addOnWmCommand(WmFunc f)
{
	onWmCommand.push_back(f);
}



void WMHandler::handleOnCreateFunc(HWND hwnd)
{
	if (onCreateFunc != nullptr)
		onCreateFunc(hwnd);
}



bool WMHandler::handleWmNotify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < onWmCommand.size(); i++)
	{
		if (onWmNotify[i](hwnd, wParam, lParam))
			return true;
	}
	return false;
}



bool WMHandler::handleWmCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < onWmCommand.size(); i++)
	{
		if (onWmCommand[i](hwnd, wParam, lParam))
			return true;
	}
	return false;
}