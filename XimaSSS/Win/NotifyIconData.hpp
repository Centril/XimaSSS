#ifndef _NOTIFYICONDATA_HPP // Inclution guard
#define _NOTIFYICONDATA_HPP

#ifdef WIN32
namespace Win
{
	class SysTrayPanel;
	class NotifyIconData
	{
		friend class SysTrayPanel;
	private:
		Window &_window;
		string m_tip;
		unsigned int m_flags;
		unsigned int m_id;
		unsigned int m_cMessage;
		unsigned int m_icon;

	public:
		enum IDs
		{
			ID = 5000,
			NID_CBM = ID + 1
		};

		NotifyIconData(Window &_w, const string &_tip, const unsigned int &_icon, const unsigned int &_uFlags, const unsigned int &_uID, const unsigned int &_uCallbackMessage);
		NotifyIconData& NotifyIconData::operator=(const NotifyIconData& _rhs);
		~NotifyIconData(void);

		void close(void);
		bool show(void);
		bool hide(void);
	};
}
#endif
#endif