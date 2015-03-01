#ifndef _GUI_HPP // Inclution guard
#define _GUI_HPP

#ifdef WIN32
#include "ImageMenuVertical.hpp"
#include "OptionsHandler.hpp"
#include "Dialog.class.hpp"

namespace Win
{
	class GUI : public Control
	{
		friend class Main;
	private:
		class Main &m_refMain;
		ImageMenuVertical<GUI> m_menu;
		static bool is_saving;

		Window m_window;
		Window _save;
		Dialog _general;
		Dialog _image;
		Dialog _ftp;
		Dialog _date;
		Dialog _sp;

	public:
		GUI(Win::Main &_refMain);
		GUI& operator=(const GUI& _rhs);
		~GUI(void);
		bool close(void);
		void save(void);
		long proc(Window &_window, const unsigned int &_message, const unsigned int &_wParam, const long &_lParam);
		void paint(HDC& _dc);

		void onClickGeneral(void);
		void onClickImage(void);
		void onClickFTP(void);
		void onClickDate(void);
		void onClickSP(void);
		void onLeaveGeneral(void);
		void onLeaveImage(void);
		void onLeaveFTP(void);
		void onLeaveDate(void);
		void onLeaveSP(void);

	};
}
#endif
#endif