#include "StdAfx.hpp"
#include "Win/Main.hpp"

__inline long __stdcall WinProc2(HWND _hWnd, unsigned int _message, unsigned int _wParam, long _lParam)
{
	Win::Window _window(_hWnd);

	Win::Control* c = _window.getControl();
	if(c)
		return c->proc(_window, _message, _wParam, _lParam);
	else
		return DefWindowProc(_hWnd, _message, _wParam, _lParam);
}

__inline long __stdcall WinProc(HWND _hWnd, unsigned int _message, unsigned int _wParam, long _lParam)
{
	if(_message == WM_NCCREATE)
	{
		Win::Window _w(_hWnd);
		_w.setUserData( ((LPCREATESTRUCT)_lParam)->lpCreateParams);
		_w.setProc(WinProc2);
		return WinProc2(_hWnd, _message, _wParam, _lParam);
	}
	else
		return DefWindowProc(_hWnd, _message, _wParam, _lParam);
}

__int32 __stdcall WinMain(HINSTANCE__* _hInst, HINSTANCE__* _prevHInst, char * _args, int _cmdShow)
{
	UNREFERENCED_PARAMETER(_prevHInst);
	UNREFERENCED_PARAMETER(_args);
	UNREFERENCED_PARAMETER(_cmdShow);

	{
		// Ensure single instance of [...] WinMain([...])
		if(!CreateMutex(0, 1, "XimaSSS"))
			return 1;
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			SetForegroundWindow(FindWindow("XimaSSS", "XimaSSS Options"));
			return 1;
		}
	}

	unsigned long gfxToken;
	{
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&gfxToken, &gdiplusStartupInput, 0);
	}
	{
		srand(static_cast<unsigned int>(time(0)));
	}

	// PROGRAM START!
	Win::Main _main;
	if(!_main.initialize(&WinProc, _hInst, "XimaSSS"))
		return 1;

	MSG message;
    while(int result = GetMessage(&message, 0, 0, 0) > 0)
    {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	// SHUTDOWN
	GdiplusShutdown(gfxToken);
	return (int)message.wParam;
}