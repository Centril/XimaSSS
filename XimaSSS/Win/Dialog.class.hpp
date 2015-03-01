#ifndef _DIALOG_HPP // Inclution guard
#define _DIALOG_HPP

#ifdef WIN32
#include "Window.class.hpp"

namespace Win
{
	class Dialog : public Window
	{
	public:
		Dialog(void);
		Dialog(HWND _hWnd);
		Dialog(const Window &_w);
		~Dialog(void);

		bool create(const Window &_parent, const char* _template, DLGPROC proc, const long &_lParam = -1, HINSTANCE _hInst = GetModuleHandle(0));
		static int messageBox(const Window &_parent, const string &_text, const string &_caption, const unsigned int &_type);

		bool setItemText(const int &_itemID, const string &_text);
		string getItemText(const int &_itemID);
		Window getItem(const int &_itemID);
	};
}

#endif
#endif