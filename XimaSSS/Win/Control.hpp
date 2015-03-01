#ifndef _CONTROL_HPP // Inclution guard
#define _CONTROL_HPP

namespace Win
{
	class Window;
	class Control
	{
	public:
		Control(void)
		{
		}

		virtual ~Control(void)
		{
		}

		virtual long proc(Window &_window, const unsigned int &_message, const unsigned int &_wParam, const long &_lParam) = 0;

	};
}
#endif