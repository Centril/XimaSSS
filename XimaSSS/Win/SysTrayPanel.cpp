#ifdef WIN32

#include "..\StdAfx.hpp"
#include "SysTrayPanel.hpp"

namespace Win
{
	SysTrayPanel::SysTrayPanel(Window &_w, const string &_tip, const unsigned int &_icon, const unsigned int &_uFlags,
		const unsigned int &_uID, const unsigned int &_uCallbackMessage)
		: m_nid(_w, _tip, _icon, _uFlags, _uID, _uCallbackMessage)
	{
	}

	SysTrayPanel& SysTrayPanel::operator=(const SysTrayPanel& _rhs)
	{
		if(this == &_rhs)
			return *this;

		this->m_nid = _rhs.m_nid;
		return *this;
	}

	SysTrayPanel::~SysTrayPanel(void)
	{
	}

	bool SysTrayPanel::run(void)
	{
		return this->m_nid.show();
	}

	int SysTrayPanel::route(const long &_lParam)
	{
		switch(_lParam)
		{
		case WM_RBUTTONDOWN:
			return this->showMenu();
			break;

		case WM_LBUTTONDOWN:	// WM_LBUTTONDOWN fires OpenGUI
			return OpenGUI;
			break;
		default:
			return 0;
		}
	}

	__inline int SysTrayPanel::showMenu(void)
	{
		POINT pt;
		GetCursorPos(&pt);

		HMENU hTrayMenu = CreatePopupMenu();
		{
			int MenuItemCount = AppendMenu(hTrayMenu, MF_STRING, OpenGUI, "Options") > 0 ? 1 : 0;
			SetMenuDefaultItem(hTrayMenu, OpenGUI, 0);
			MenuItemCount += AppendMenu(hTrayMenu, MF_STRING, MultiMonitor, "Capture All Screens") > 0 ? 1 : 0;
			MenuItemCount += AppendMenu(hTrayMenu, MF_STRING, PrimaryScreen, "Capture Primary Screen") > 0 ? 1 : 0;
			MenuItemCount += AppendMenu(hTrayMenu, MF_STRING, CaptureWindow, "Capture Window") > 0 ? 1 : 0;
			MenuItemCount += AppendMenu(hTrayMenu, MF_STRING, Foreground, "Capture Foreground Window") > 0 ? 1 : 0;
			MenuItemCount += AppendMenu(hTrayMenu, MF_STRING, Area, "Capture Area") > 0 ? 1 : 0;
			MenuItemCount += AppendMenu(hTrayMenu, MF_STRING, WorkingArea, "Capture Working Area") > 0 ? 1 : 0;
			MenuItemCount += AppendMenu(hTrayMenu, MF_STRING, FTPFiles, "FTP Files") > 0 ? 1 : 0;
			MenuItemCount += AppendMenu(hTrayMenu, MF_STRING, Exit, "Exit") > 0 ? 1 : 0;
			if(MenuItemCount != 9)
			{
				DestroyMenu(hTrayMenu);
				return 0;
			}
		}

		this->m_nid._window.toForeground(); /* so stray clicks and <Esc> will cancel menu */
		int menuCommand = TrackPopupMenu(hTrayMenu, TPM_VERNEGANIMATION | TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY, pt.x, pt.y, 0, this->m_nid._window, 0);
		this->m_nid._window.postMessage(WM_NULL, 0, 0);
		DestroyMenu(hTrayMenu);

		return menuCommand;
	}
}
#endif