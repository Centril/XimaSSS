#ifndef _SNAPPER_HPP
#define _SNAPPER_HPP

#include "Window.class.hpp"

namespace Win
{
	class Snapper
	{
	private:
		Window &m_refMainWindow;
		const struct Options &options;

	public:
		Snapper(Options &_o, Window& _w);
		Snapper& operator =(const Snapper &_rhs);
		~Snapper(void);

		// h = high priority
		// l = low priority
		// p = parent
		// c = child
		void multimonitor(void);	// (h)(p)(Key: PrintScrn)
		void primary(void);			// (h)(c)(Key: Alt + PrintScrn)

		void selectArea(bool &flag);		// (h)(p)(Key: Control + PrintScrn)
		void abortArea(void);
		void area(void);

		void workingarea(void);		// (l)(c)(Key: Alt + Control + PrintScrn)

		void selectWindow(bool &flag);	// (h)(p)(Key: Shift + PrintScrn)
		void abortWindow(void);
		void window(void);

		void foreground(void);		// (l)(c)(Key: Shift + Control + PrintScrn)
	};
}
#endif