#ifndef _WINDOW_HPP // Inclution guard
#define _WINDOW_HPP

#ifdef WIN32

#ifndef MAX_CLASSNAME_LEN
	#define MAX_CLASSNAME_LEN 50
#endif

namespace Win
{
	class WindowException
	{
	protected:
		const unsigned long m_code;

		WindowException& operator=(const WindowException& _rhs)
		{
			if(this == &_rhs)
				return *this;
		}

	public:
		WindowException(void) : m_code(GetLastError())
		{
		}

		WindowException(const WindowException& _we) : m_code(_we.m_code)
		{
		}

		~WindowException(void)
		{
		}

		const int what(void) const
		{
			return this->m_code;
		}

	};

	class Window
	{
	protected:
		HWND__* m_hwnd;
		static int _nexthotkeyid;

	public:
		Window(void);
		Window(const Window &_w);
		Window(HWND__* _hwnd);
		~Window(void);

		void create(const unsigned long &_exStyles, const unsigned long &_styles,
			const string &_className, const string &_windowName,
			const int &_xPos, const int &_yPos, const int &_nWidth, const int &_nHeight,
			Win::Control* _lpParam = 0, HWND__* _hWndParent = 0, HMENU__* _hMenu = 0, HINSTANCE__* _hInst = GetModuleHandle(0));

		void show(const int &cmdShow = SW_SHOW);
		void hide(void);
		void update(void);
		void validate(const RECT &_r);
		void animate(const unsigned long &_time, const unsigned long &_flags);
		void setPos(const int &_insertAfter, const int &_x, const int &_y, const int &_cx, const int &_cy, const unsigned int &_flags = SWP_NOZORDER);
		void move(const int &_x, const int &_y, const int &_width, const int &_height, const bool &_repaint);
		void toTop(void);
		void toForeground(void);
		static Window getForeground(void);
		static Window getDesktop(void);
		Window getTopChild(void);
		Window getNext(void);
		Window getPrevious(void);
		void close(void);
		void destroy(void);

		HWND__* operator*(void);
		operator HWND__*();

		bool find(const string &_class, const string &_name);
		string getText(void);
		string getClassName(void);
		string getModuleFilename(void);
		string getFilename(void);
		Window getParent(void);
		void getClientRect(RECT* _prect);
		void getRect(RECT* _prect);
		bool containsPoint(const int &xPos, const int &yPos);

		Window setFocus(void);
		static Window getFocus(void);
		Window setActiveWindow(void);
		static Window getActiveWindow(void);
		bool enable(void);
		bool disable(void);
		bool isEnabled(void);
		bool isWindow(void);
		void screenToClient(POINT* _p);
		void clientToScreen(POINT* _p);

		const int registerHotKey(const unsigned int &_modifiers, const unsigned int &_vk);
		void unregisterHotKey(const int &_id);

		long sendMessage(const unsigned int &_message, const unsigned int &_wParam, const long &_lParam);
		void postMessage(const unsigned int &_message, const unsigned int &_wParam, const long &_lParam);

		long getSetting(const int &_whatSetting);
		long setSetting(const int &_whatSetting, const long &_newVal);
		Win::Control* getControl(void);
		WNDPROC setProc(WNDPROC _newProc);
		void* setUserData(void* _newUserData);

	};
}
#endif
#endif