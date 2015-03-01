#ifdef WIN32

#include "..\StdAfx.hpp"

#include "Dialog.class.hpp"

namespace Win
{
	Dialog::Dialog(void)
	{
	}

	Dialog::Dialog(HWND _hWnd) : Window(_hWnd)
	{
	}

	Dialog::Dialog(const Window &_w) : Window(_w)
	{
	}

	Dialog::~Dialog(void)
	{
	}

	bool Dialog::create(const Window &_parent, const char* _template, DLGPROC _proc, const long &_lParam, HINSTANCE _hInst)
	{
		this->m_hwnd = CreateDialogParam(_hInst, _template, const_cast<Window&>(_parent), _proc, _lParam);
		return this->m_hwnd != 0 ? true : false;
	}

	int Dialog::messageBox(const Window &_parent, const string &_text, const string &_caption, const unsigned int &_type)
	{
		return MessageBox(const_cast<Window&>(_parent), _text.c_str(), _caption.c_str(), _type);
	}

	bool Dialog::setItemText(const int &_itemID, const string &_text)
	{
		return SetDlgItemText(this->m_hwnd, _itemID, _text.c_str()) == 1 ? true : false;
	}

	string Dialog::getItemText(const int &_itemID)
	{
		char buffer[512];
		if(GetDlgItemText(this->m_hwnd, _itemID, buffer, 512) > 0)
			return buffer;
		else
			return "";
	}

	Window Dialog::getItem(const int &_itemID)
	{
		return Window(GetDlgItem(this->m_hwnd, _itemID));
	}
}
#endif