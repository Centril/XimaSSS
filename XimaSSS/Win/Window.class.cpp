#ifdef WIN32

#include "..\StdAfx.hpp"

#include "Window.class.hpp"
#include <psapi.h> // for GetModuleFileNameEx(...)

namespace Win
{
	Window::Window(void)
	{
	}

	Window::Window(HWND__* _hwnd) : m_hwnd(_hwnd)
	{
		if(this->m_hwnd == 0)
			throw WindowException();
	}

	Window::Window(const Window &_w) : m_hwnd(_w.m_hwnd)
	{
	}

	Window::~Window(void)
	{
	}

	void Window::create(const unsigned long &_exStyles, const unsigned long &_styles,
		const string &_className, const string &_windowName,
		const int &_xPos, const int &_yPos, const int &_nWidth, const int &_nHeight,
		Win::Control* _lpParam, HWND__* _hWndParent, HMENU__* _hMenu, HINSTANCE__* _hInst)
	{
		this->m_hwnd = CreateWindowEx
		(
			_exStyles,
			_className.c_str(),
			_windowName.c_str(),
			_styles,
			_xPos, _yPos, _nWidth, _nHeight,
			_hWndParent,
			_hMenu,
			_hInst,
			_lpParam
		);

		if(this->m_hwnd == 0)
			throw WindowException();
	}

	void Window::show(const int &cmdShow)
	{
		ShowWindow(this->m_hwnd, cmdShow);
	}

	void Window::hide(void)
	{
		this->show(SW_HIDE);
	}

	void Window::update(void)
	{
		if(UpdateWindow(this->m_hwnd) == 0)
			throw WindowException(); 
	}

	void Window::validate(const RECT &_r)
	{
		if(ValidateRect(this->m_hwnd, &_r) == 0)
			throw WindowException();
	}

	void Window::setPos(const int &_insertAfter, const int &_x, const int &_y, const int &_cx, const int &_cy, const unsigned int &_flags)
	{
		if(SetWindowPos(this->m_hwnd, (HWND)_insertAfter, _x, _y, _cx, _cy, _flags) == 0)
			throw WindowException();
	}

	void Window::move(const int &_x, const int &_y, const int &_width, const int &_height, const bool &_repaint)
	{
		if(MoveWindow(this->m_hwnd, _x, _y, _width, _height, _repaint) == 0)
			throw WindowException();
	}

	void Window::animate(const unsigned long &_time, const unsigned long &_flags)
	{
		if(AnimateWindow(this->m_hwnd, _time, _flags) == 0)
			throw WindowException();
	}

	void Window::toTop(void)
	{
		if(BringWindowToTop(this->m_hwnd) == 0)
			throw WindowException();
	}

	void Window::toForeground(void)
	{
		if(SetForegroundWindow(this->m_hwnd) == 0)
			throw WindowException();
	}

	Window Window::getForeground(void)
	{
		return Window(GetForegroundWindow());
	}

	Window Window::getDesktop(void)
	{
		return Window(GetDesktopWindow());
	}

	Window Window::getTopChild(void)
	{
		return Window(GetTopWindow(this->m_hwnd));
	}

	Window Window::getNext(void)
	{
		return Window(GetNextWindow(this->m_hwnd, GW_HWNDNEXT));
	}

	Window Window::getPrevious(void)
	{
		return Window(GetNextWindow(this->m_hwnd, GW_HWNDPREV));
	}

	void Window::close(void)
	{
		if(CloseWindow(this->m_hwnd) == 0)
			throw WindowException();
	}

	void Window::destroy(void)
	{
		if(DestroyWindow(this->m_hwnd) == 0)
			throw WindowException();
	}

	HWND__* Window::operator*(void)
	{
		return this->m_hwnd;
	}

	Window::operator HWND__*()
	{
		return this->m_hwnd;
	}

	string Window::getText(void)
	{
		char buffer[100];
		GetWindowText(this->m_hwnd, buffer, 100);
		return buffer;
	}

	string Window::getClassName(void)
	{
		char buffer[MAX_CLASSNAME_LEN + 1];
		if(GetClassName(this->m_hwnd, buffer, MAX_CLASSNAME_LEN + 1) == 0)
			throw WindowException();
		return buffer;
	}

	string Window::getModuleFilename(void)
	{
		char buffer[512];
		if(GetWindowModuleFileName(this->m_hwnd, buffer, sizeof buffer) == 0)
			throw WindowException();
		return buffer;
	}

	string Window::getFilename(void)
	{
		string processName = "";
		HANDLE process;
		{
			unsigned long pID;
			GetWindowThreadProcessId(this->m_hwnd, &pID);
			process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pID);
		}
		{
			char buffer[256];
			GetModuleFileNameEx(process, 0, buffer, 256);
			processName = buffer;
		}
		CloseHandle(process);
		return processName;
	}

	void Window::getClientRect(RECT* _prect)
	{
		if(GetClientRect(this->m_hwnd, _prect) == 0)
			throw WindowException();
	}

	void Window::getRect(RECT* _prect)
	{
		if(GetWindowRect(this->m_hwnd, _prect) == 0)
			throw WindowException();
	}

	bool Window::containsPoint(const int &xPos, const int &yPos)
	{
		try
		{
			POINT _point;
			_point.x = xPos;
			_point.y = yPos;
			RECT _rect;
			this->getClientRect(&_rect);
			return PtInRect(&_rect, _point) != 0 ? true: false;
		}
		catch(WindowException &_we)
		{
			throw _we;
		}
	}

	Window Window::getParent(void)
	{
		return Window((HWND__*)GetWindowLongPtr(this->m_hwnd, GWLP_HWNDPARENT));
	}

	bool Window::find(const string &_class, const string &_name)
	{
		HWND _hWnd = FindWindow(_class.c_str(), _name.c_str());
		if(!_hWnd)
			return false;

		this->m_hwnd = _hWnd;
		return true;
	}

	Window Window::setFocus(void)
	{
		return Window(SetFocus(this->m_hwnd));
	}

	Window Window::getFocus(void)
	{
		return Window(GetFocus());
	}

	Window Window::setActiveWindow(void)
	{
		return Window(SetActiveWindow(this->m_hwnd));
	}

	Window Window::getActiveWindow(void)
	{
		return Window(GetActiveWindow());
	}

	bool Window::enable(void)
	{
		return EnableWindow(this->m_hwnd, 1) != 0 ? true : false;
	}

	bool Window::disable(void)
	{
		return EnableWindow(this->m_hwnd, 0) == 0 ? true : false;
	}

	bool Window::isEnabled(void)
	{
		return IsWindowEnabled(this->m_hwnd) != 0 ? true : false;
	}

	bool Window::isWindow(void)
	{
		return IsWindow(this->m_hwnd) != 0 ? true : false;
	}

	void Window::screenToClient(POINT* _p)
	{
		if(ScreenToClient(this->m_hwnd, _p) == 0)
			throw WindowException();
	}

	void Window::clientToScreen(POINT* _p)
	{
		if(ClientToScreen(this->m_hwnd, _p) == 0)
			throw WindowException();
	}

	int Window::_nexthotkeyid = 0x0000;
	const int Window::registerHotKey(const unsigned int &_modifiers, const unsigned int &_vk)
	{
		if(_nexthotkeyid == 0xC000)
			throw WindowException();
		RegisterHotKey(this->m_hwnd, _nexthotkeyid, _modifiers, _vk);
		return _nexthotkeyid++;
	}

	void Window::unregisterHotKey(const int &_id)
	{
		if(UnregisterHotKey(this->m_hwnd, _id) == 0)
			throw WindowException();
	}

	long Window::getSetting(const int &_whatSetting)
	{
		long result = GetWindowLongPtr(this->m_hwnd, _whatSetting);
		if(result == 0)
			throw WindowException();
		else
			return result;
	}

	long Window::setSetting(const int &_whatSetting, const long &_newVal)
	{
		long result = SetWindowLongPtr(this->m_hwnd, _whatSetting, _newVal);
		if(result == 0)
			throw WindowException();
		else
			return result;
	}

	Win::Control* Window::getControl(void)
	{
		Win::Control* _c = reinterpret_cast<Win::Control*>(GetWindowLongPtr(this->m_hwnd, GWLP_USERDATA));
		assert(_c);
		return _c;
	}

	WNDPROC Window::setProc(WNDPROC _newProc)
	{
		return (WNDPROC)SetWindowLongPtr(this->m_hwnd, GWLP_WNDPROC, (long) _newProc);
	}

	void* Window::setUserData(void* _newUserData)
	{
		return (void*)SetWindowLongPtr(this->m_hwnd, GWLP_USERDATA, (long) _newUserData);
	}

	long Window::sendMessage(const unsigned int &_message, const unsigned int &_wParam, const long &_lParam)
	{
		return SendMessage(this->m_hwnd, _message, _wParam, _lParam);
	}

	void Window::postMessage(const unsigned int &_message, const unsigned int &_wParam, const long &_lParam)
	{
		if(PostMessage(this->m_hwnd, _message, _wParam, _lParam) == 0)
			throw WindowException();
	}
}
#endif