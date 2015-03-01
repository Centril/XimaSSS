#include <windows.h>
#include <set>
#include <algorithm>
#include <ctime>
using namespace std;

#include "XCaptureHelper.h"
#include "../XimaSSS/WinMessages.hpp"
#include "../XimaSSS/Win/Control.hpp"
#include "../XimaSSS/Win/Window.class.hpp"
#include "../XimaSSS/Win/Class.class.hpp"
#include "MMTimer.h"

static HINSTANCE gInst;

/*                  */
/*  Window Section  */
/*                  */
class Blinker : public CMMTimerListener
{
private:
	bool state;
	HWND current;
	set<HWND> prev;

public:
	static const int n = 13;
	CMMTimer timer;

	Blinker(void) : state(true), current(0)
	{
	}

	void fill(void)
	{
		RECT _bounds;
		GetWindowRect(this->current, &_bounds);
		_bounds.bottom -= _bounds.top; _bounds.top = 0;
		_bounds.right -= _bounds.left; _bounds.left = 0;

		RECT rect;
		HDC _dc = GetWindowDC(this->current);
		rect.top = 0; rect.left = 0; rect.bottom = _bounds.bottom; rect.right = this->n;
		InvertRect(_dc, &rect);
		rect.left = this->n; rect.bottom = this->n; rect.right = _bounds.right;
		InvertRect(_dc, &rect);
		rect.top = this->n; rect.left = rect.right - this->n; rect.bottom = _bounds.bottom;
		InvertRect(_dc, &rect);
		rect.left = this->n; rect.right -= this->n; rect.top = rect.bottom - this->n;
		InvertRect(_dc, &rect);
		ReleaseDC(this->current, _dc);

		this->state = false;
	}

	void clear(void)
	{
		RECT _bounds;
		GetWindowRect(this->current, &_bounds);
		_bounds.bottom -= _bounds.top; _bounds.top = 0;
		_bounds.right -= _bounds.left; _bounds.left = 0;

		RECT rect;
		HDC _dc = GetWindowDC(this->current);
		rect.top = 0; rect.left = 0; rect.bottom = _bounds.bottom; rect.right = this->n;
		InvertRect(_dc, &rect);
		rect.left = this->n; rect.bottom = this->n; rect.right = _bounds.right;
		InvertRect(_dc, &rect);
		rect.top = this->n; rect.left = rect.right - this->n; rect.bottom = _bounds.bottom;
		InvertRect(_dc, &rect);
		rect.left = this->n; rect.right -= this->n; rect.top = rect.bottom - this->n;
		InvertRect(_dc, &rect);
		ReleaseDC(this->current, _dc);

		set<HWND>::iterator iter;
		for(iter = this->prev.begin(); iter != this->prev.end(); iter++)
			this->fullclear(*iter);
		this->state = true;
	}

	set<HWND>& getPrevs(void)
	{
		return this->prev;
	}

	void fullclear(HWND _hwnd)
	{
		RECT _bounds;
		GetWindowRect(_hwnd, &_bounds);
		_bounds.bottom -= _bounds.top; _bounds.top = 0;
		_bounds.right -= _bounds.left; _bounds.left = 0;

		RECT rect;
		rect.top = 0; rect.left = 0; rect.bottom = _bounds.bottom; rect.right = this->n;
		RedrawWindow(_hwnd, &rect, 0, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
		rect.left = this->n; rect.bottom = this->n; rect.right = _bounds.right;
		RedrawWindow(_hwnd, &rect, 0, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
		rect.top = this->n; rect.left = rect.right - this->n; rect.bottom = _bounds.bottom;
		RedrawWindow(_hwnd, &rect, 0, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
		rect.left = this->n; rect.right -= this->n; rect.top = rect.bottom - this->n;
		RedrawWindow(_hwnd, &rect, 0, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW );
	}

	void setCurrent(HWND _hwnd)
	{
		this->current = _hwnd;
		this->prev.insert(this->getCurrent());
	}

	HWND getCurrent(void)
	{
		return this->current;
	}

	void change(HWND _hwnd)
	{
		this->timer.Stop();

		if(this->timer.GetCount() % 2 != 0)
			this->clear();
		else
			this->state = true;

		this->setCurrent(_hwnd);

		this->timer.Reset();
		this->timer.Start(250, 0);
	}

	void Update(CMMTimer&)
	{
		if(this->state)
			this->fill();
		else
			this->clear();
	}
} blinker;

class Checker : public CMMTimerListener
{
public:
	CMMTimer timer;

	void Update(CMMTimer&)
	{
		POINT pt;
		GetCursorPos(&pt);
		HWND _hwnd = WindowFromPoint(pt);
		if(_hwnd == 0 || _hwnd == blinker.getCurrent())
			return;

		blinker.change(_hwnd);
	}
} checker;

HHOOK WindowMouseHook;
LRESULT __stdcall WindowMouseHookProc(int nCode, unsigned int _wParam, long _lParam)
{
	static HWND hWnd = FindWindow("XimaSSS", "");

	if(nCode != HC_ACTION)
		return CallNextHookEx(WindowMouseHook, nCode, _wParam, _lParam);

	switch(_wParam)
	{
	case WM_LBUTTONUP:
		PostMessage(hWnd, CW_WindowSelected, 0, 0);
		break;
	case WM_RBUTTONUP:
		PostMessage(hWnd, CW_Abort, 0, 0);
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	default:
		return CallNextHookEx(WindowMouseHook, nCode, _wParam, _lParam);
	}
	return 1;
}

HHOOK WindowKBHook;
struct kbstruct {unsigned long vkCode;};
LRESULT __stdcall WindowKBHookProc(int nCode, unsigned int _wParam, long _lParam)
{
	static HWND hWnd = FindWindow("XimaSSS", "");

	if(nCode != HC_ACTION)
		return CallNextHookEx(WindowMouseHook, nCode, _wParam, _lParam);

	switch(_wParam)
	{
	case WM_KEYDOWN:
		switch(reinterpret_cast<kbstruct*>(_lParam)->vkCode)
		{
		case VK_SPACE:
			PostMessage(hWnd, CW_WindowSelected, 0, 0);
			break;
		case VK_ESCAPE:
			PostMessage(hWnd, CW_Abort, 0, 0);
			break;
		case VK_UP:
			mouse_event(MOUSEEVENTF_MOVE, 0, static_cast<unsigned long>(-5), 0, 0);
			break;
		case VK_DOWN:
			mouse_event(MOUSEEVENTF_MOVE, 0, 5, 0, 0);
			break;
		case VK_RIGHT:
			mouse_event(MOUSEEVENTF_MOVE, 5, 0, 0, 0);
			break;
		case VK_LEFT:
			mouse_event(MOUSEEVENTF_MOVE, static_cast<unsigned long>(-5), 0, 0, 0);
			break;
		default:
			return CallNextHookEx(WindowKBHook, nCode, _wParam, _lParam);
		}
		break;
	default:
		return CallNextHookEx(WindowKBHook, nCode, _wParam, _lParam);
	}
	return 1;
}

API bool __stdcall BeginWindowSelection(void)
{
	WindowMouseHook = SetWindowsHookEx(14, WindowMouseHookProc, gInst, 0);
	WindowKBHook = SetWindowsHookEx(13, WindowKBHookProc, gInst, 0);
	POINT pt;
	GetCursorPos(&pt);
	blinker.setCurrent(WindowFromPoint(pt));

	checker.timer.AttachListener(checker);
	blinker.timer.AttachListener(blinker);
	checker.timer.Start(1, 0);
	blinker.timer.Start(250, 0);
	return (WindowMouseHook != 0 && WindowKBHook != 0) ? true : false;
}

API HWND __stdcall EndWindowSelection(void)
{
	blinker.timer.Stop();
	checker.timer.Stop();
	blinker.timer.DetachListener(blinker);
	checker.timer.DetachListener(checker);

	set<HWND> prev = blinker.getPrevs();
	set<HWND>::iterator iter;
	for(iter = prev.begin(); iter != prev.end(); iter++)
		blinker.fullclear(*iter);

	if(!UnhookWindowsHookEx(WindowMouseHook))
		return 0;
	if(!UnhookWindowsHookEx(WindowKBHook))
		return 0;
	return blinker.getCurrent();
}
/*                */
/*  Area Section  */
/*                */
#define AREA_ABORT 1
#define AREA_CLICK 2
#define AREA_POSMOVED 3

HHOOK AreaMouseHook;
HHOOK AreaKBHook;

static const int borderSize = 5;
static bool sizing(false);
static POINT startPos;
static POINT endPos;
static Win::Window ghostWindow;

long __stdcall AreaWinProc(HWND _hWnd, unsigned int _message, unsigned int _wParam, long _lParam)
{
	switch(_message)
	{
	case WM_ERASEBKGND:
	{
		return 1;
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hMarkerDC = BeginPaint(_hWnd, &ps);

		{
			RECT rect;

			HBRUSH hBorder = CreateSolidBrush(RGB(255, 0, 0));

			// tl->tr
			rect.left = startPos.x;
			rect.top = startPos.y;
			rect.right = endPos.x;
			rect.bottom = startPos.y > endPos.y ? startPos.y - borderSize : startPos.y + borderSize;
			FillRect(hMarkerDC, &rect, hBorder);

			// tl->bl
			rect.left = startPos.x;
			rect.top = startPos.y;
			rect.right = startPos.x > endPos.x ? startPos.x - borderSize : startPos.x + borderSize;
			rect.bottom = endPos.y;
			FillRect(hMarkerDC, &rect, hBorder);

			// br->bl
			rect.left = startPos.x;
			rect.top = endPos.y > startPos.y ? endPos.y - borderSize : endPos.y + borderSize;
			rect.right = endPos.x;
			rect.bottom = endPos.y;
			FillRect(hMarkerDC, &rect, hBorder);

			// br->tr
			rect.left = endPos.x > startPos.x ? endPos.x - borderSize : endPos.x + borderSize;
			rect.top = startPos.y;
			rect.right = endPos.x;
			rect.bottom = endPos.y;
			FillRect(hMarkerDC, &rect, hBorder);

			DeleteObject(hBorder);
		}

		EndPaint(_hWnd, &ps);
		break;
	}
	default:
		return DefWindowProc(_hWnd, _message, _wParam, _lParam);
	}
	return 0;
}

long __stdcall AreaCaptureProc(unsigned int _message, long _lParam)
{
	static HWND hWnd = FindWindow("XimaSSS", "");

	switch(_message)
	{
	case AREA_ABORT:
	{
		sizing = false;

		RECT rect;
		{
			rect.left = startPos.x;
			rect.top = startPos.y;
			rect.right = endPos.x;
			rect.bottom = startPos.y > endPos.y ? startPos.y - borderSize : startPos.y + borderSize;
			InvalidateRect(ghostWindow, &rect, TRUE);

			rect.left = startPos.x;
			rect.top = startPos.y;
			rect.right = startPos.x > endPos.x ? startPos.x - borderSize : startPos.x + borderSize;
			rect.bottom = endPos.y;
			InvalidateRect(ghostWindow, &rect, TRUE);

			rect.left = startPos.x;
			rect.top = endPos.y > startPos.y ? endPos.y - borderSize : endPos.y + borderSize;
			rect.right = endPos.x;
			rect.bottom = endPos.y;
			InvalidateRect(ghostWindow, &rect, TRUE);

			rect.left = endPos.x > startPos.x ? endPos.x - borderSize : endPos.x + borderSize;
			rect.top = startPos.y;
			rect.right = endPos.x;
			rect.bottom = endPos.y;
			InvalidateRect(ghostWindow, &rect, TRUE);

			UpdateWindow(ghostWindow);
		}

		startPos.x = startPos.y = 0;
		endPos.x = endPos.y = 0;
		if(ghostWindow.isWindow())
			ghostWindow.destroy();

		PostMessage(hWnd, Area_Abort, 0, 0);
		break;
	}
	case AREA_CLICK:
	{
		if(sizing)
		{
			sizing = false;

			RECT rect;
			{
				rect.left = startPos.x;
				rect.top = startPos.y;
				rect.right = endPos.x;
				rect.bottom = startPos.y > endPos.y ? startPos.y - borderSize : startPos.y + borderSize;
				InvalidateRect(ghostWindow, &rect, TRUE);

				rect.left = startPos.x;
				rect.top = startPos.y;
				rect.right = startPos.x > endPos.x ? startPos.x - borderSize : startPos.x + borderSize;
				rect.bottom = endPos.y;
				InvalidateRect(ghostWindow, &rect, TRUE);

				rect.left = startPos.x;
				rect.top = endPos.y > startPos.y ? endPos.y - borderSize : endPos.y + borderSize;
				rect.right = endPos.x;
				rect.bottom = endPos.y;
				InvalidateRect(ghostWindow, &rect, TRUE);

				rect.left = endPos.x > startPos.x ? endPos.x - borderSize : endPos.x + borderSize;
				rect.top = startPos.y;
				rect.right = endPos.x;
				rect.bottom = endPos.y;
				InvalidateRect(ghostWindow, &rect, TRUE);

				UpdateWindow(ghostWindow);
			}

			startPos.x = startPos.y = 0;
			endPos.x = endPos.y = 0;
			if(ghostWindow.isWindow())
				ghostWindow.destroy();
		}
		else
		{
			if(!GetCursorPos(&startPos))
				return AreaCaptureProc(AREA_ABORT, 0);
			sizing = true;

			ghostWindow.create(0, WS_VISIBLE | WS_CHILD, "Dummy", "", 0, 0, 1280, 1024, 0, GetDesktopWindow());
		}
		break;
	}
	case AREA_POSMOVED:
	{
		if(!sizing)
			return 1;

		RECT rect;
		{
			rect.left = startPos.x;
			rect.top = startPos.y;
			rect.right = endPos.x;
			rect.bottom = startPos.y > endPos.y ? startPos.y - borderSize : startPos.y + borderSize;
			InvalidateRect(ghostWindow, &rect, TRUE);

			rect.left = startPos.x;
			rect.top = startPos.y;
			rect.right = startPos.x > endPos.x ? startPos.x - borderSize : startPos.x + borderSize;
			rect.bottom = endPos.y;
			InvalidateRect(ghostWindow, &rect, TRUE);

			rect.left = startPos.x;
			rect.top = endPos.y > startPos.y ? endPos.y - borderSize : endPos.y + borderSize;
			rect.right = endPos.x;
			rect.bottom = endPos.y;
			InvalidateRect(ghostWindow, &rect, TRUE);

			rect.left = endPos.x > startPos.x ? endPos.x - borderSize : endPos.x + borderSize;
			rect.top = startPos.y;
			rect.right = endPos.x;
			rect.bottom = endPos.y;
			InvalidateRect(ghostWindow, &rect, TRUE);

			UpdateWindow(ghostWindow);
		}

		GetCursorPos(&endPos);
		break;
	}
	}
	return 1;
}

LRESULT __stdcall AreaMouseHookProc(int nCode, unsigned int _wParam, long _lParam)
{
	if(nCode != HC_ACTION)
		return CallNextHookEx(AreaMouseHook, nCode, _wParam, _lParam);

	switch(_wParam)
	{
	case WM_RBUTTONUP:
		return AreaCaptureProc(AREA_ABORT, 0);
		break;
	case WM_LBUTTONUP:
		return AreaCaptureProc(AREA_CLICK, 0);
		break;
	case WM_LBUTTONDOWN:
		return AreaCaptureProc(AREA_CLICK, 0);

		break;
	case WM_MOUSEMOVE:
		AreaCaptureProc(AREA_POSMOVED, 0);
		return 0;
		break;
	default:
		return CallNextHookEx(AreaMouseHook, nCode, _wParam, _lParam);
	}
	return 1;
}

LRESULT __stdcall AreaKBHookProc(int nCode, unsigned int _wParam, long _lParam)
{
	if(nCode != HC_ACTION)
		return CallNextHookEx(AreaKBHook, nCode, _wParam, _lParam);

	switch(_wParam)
	{
	case WM_KEYDOWN:
		switch(reinterpret_cast<kbstruct*>(_lParam)->vkCode)
		{
		case VK_SPACE:
			//PostMessage(hWnd, CW_WindowSelected, 0, 0);
			break;
		case VK_ESCAPE:
			return AreaCaptureProc(AREA_ABORT, 0);
			break;
		default:
			return CallNextHookEx(AreaKBHook, nCode, _wParam, _lParam);
		}
		break;
	default:
		return CallNextHookEx(AreaKBHook, nCode, _wParam, _lParam);
	}
	return 1;
}

API bool __stdcall BeginAreaSelection(void)
{
	{
		WNDCLASSEX _class;
		_class.cbSize = sizeof WNDCLASSEX;
		_class.hInstance = GetModuleHandle(0);
		_class.lpfnWndProc = AreaWinProc;
		_class.lpszClassName = "Dummy";
		_class.lpszMenuName = 0;
		_class.style = 0;
		_class.cbClsExtra = 0;
		_class.cbWndExtra = 0;
		_class.hbrBackground = 0;
		_class.hCursor = 0;
		_class.hIcon = 0;
		_class.hIconSm = 0;
		RegisterClassEx(&_class);
	}

	AreaMouseHook = SetWindowsHookEx(14, AreaMouseHookProc, gInst, 0);
	AreaKBHook = SetWindowsHookEx(13, AreaKBHookProc, gInst, 0);
	return (AreaMouseHook != 0 && AreaKBHook != 0) ? true : false;
}

API bool __stdcall EndAreaSelection(RECT& _rect)
{
	UnregisterClass("Dummy", GetModuleHandle(0));
	if(!UnhookWindowsHookEx(AreaMouseHook))
		return false;
	if(!UnhookWindowsHookEx(AreaKBHook))
		return false;
	return true;
}

int __stdcall DllMain(HINSTANCE hInst, unsigned long,void*)
{
	gInst = hInst;
    return 1;
}