#ifdef WIN32

#include "..\StdAfx.hpp"

#include "Window.class.hpp"
#include "NotifyIconData.hpp"
#include "shlwapi.h"

namespace Win
{
	NotifyIconData::NotifyIconData(Window &_w, const string &_tip, const unsigned int &_icon, const unsigned int &_uFlags, const unsigned int &_uID, const unsigned int &_uCallbackMessage)
		: _window(_w), m_icon(_icon), m_tip(_tip), m_flags(_uFlags), m_id(_uID), m_cMessage(_uCallbackMessage)
	{
	}

	NotifyIconData& NotifyIconData::operator=(const NotifyIconData& _rhs)
	{
		if(this == &_rhs)
			return *this;

		this->m_cMessage = _rhs.m_cMessage;
		this->m_flags = _rhs.m_flags;
		this->m_icon = _rhs.m_icon;
		this->m_id = _rhs.m_id;
		this->m_tip = _rhs.m_tip;
		this->_window = _rhs._window;
		return *this;
	}

	NotifyIconData::~NotifyIconData(void)
	{
		this->close();
	}

	void NotifyIconData::close(void)
	{
		NOTIFYICONDATA delnid;
		delnid.cbSize = NOTIFYICONDATA_V1_SIZE;
		delnid.uID = this->m_id;
		delnid.hWnd = *this->_window;

		Shell_NotifyIcon(NIM_DELETE, &delnid);
	}

	bool NotifyIconData::show(void)
	{
		NOTIFYICONDATA shownid;
		shownid.cbSize = NOTIFYICONDATA_V1_SIZE;
		shownid.hWnd = *this->_window;
		shownid.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(this->m_icon));
		strcpy_s(shownid.szTip, sizeof shownid.szTip, this->m_tip.c_str());
		shownid.uID = this->m_id;
		shownid.uCallbackMessage = this->m_cMessage;
		shownid.uFlags = this->m_flags;
		bool ret = Shell_NotifyIcon(NIM_ADD, &shownid) == 1 ? true : false;
		DestroyIcon(shownid.hIcon);
		return ret;
	}

	bool NotifyIconData::hide(void)
	{
		NOTIFYICONDATA hidenid;
		hidenid.cbSize = NOTIFYICONDATA_V1_SIZE;
		hidenid.uID = this->m_id;
		hidenid.hWnd = *this->_window;
		return Shell_NotifyIcon(NIM_DELETE, &hidenid) == 1 ? true : false;
	}
}
#endif