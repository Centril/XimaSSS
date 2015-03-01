#ifndef _MAIN_HPP // Inclution guard
#define _MAIN_HPP

#ifdef WIN32
#include "Control.hpp"
#include "Class.class.hpp"
#include "Window.class.hpp"
#include "Dialog.class.hpp"
#include "SysTrayPanel.hpp"
#include "OptionsHandler.hpp"

#include "Snapper.hpp"
#include "..\FTP\FTPUploader.class.hpp"

namespace Win
{
	class SysTrayPanel;
	class Main : public Control
	{
		friend class GUI;
	private:
		Window m_window;
		SysTrayPanel m_stp;
		Snapper m_capt;
		GUI *m_refGUI;

	public:
		Options options;
		bool selectingWindow;
		bool selectingArea;

	public:
		Main(void);
		~Main(void);

		bool initialize(WNDPROC _winProc, HINSTANCE__* _hInst, const string &_className);
		long proc(Window &_window, const unsigned int &_message, const unsigned int &_wParam, const long &_lParam);

		void ftpAll(void);
	};
}
#endif
#endif