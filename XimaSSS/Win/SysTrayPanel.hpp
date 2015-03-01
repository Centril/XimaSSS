#ifndef _SYSTRAYPANEL_HPP // Inclution guard
#define _SYSTRAYPANEL_HPP

#ifdef WIN32
#include "./Window.class.hpp"
#include "./NotifyIconData.hpp"

namespace Win
{
	class SysTrayPanel
	{
	private:
		NotifyIconData m_nid;

	public:
		SysTrayPanel(Window &_w, const string &_tip, const unsigned int &_icon, const unsigned int &_uFlags,
			const unsigned int &_uID, const unsigned int &_uCallbackMessage);
		~SysTrayPanel(void);
		SysTrayPanel& operator=(const SysTrayPanel& _rhs);

		bool run(void);
		int route(const long &_lParam);
		int showMenu(void);
	};
}
#endif
#endif