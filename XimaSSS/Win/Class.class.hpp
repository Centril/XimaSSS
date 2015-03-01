#ifndef _CLASS_HPP // Inclution guard
#define _CLASS_HPP

#ifdef WIN32
namespace Win
{
	class Class
	{
	private:
		WNDCLASSEX m_class;

	public:
		Class(void);
		Class(WNDPROC _winProc, HINSTANCE _hInst, HICON _hIcon, HBRUSH _hBG,
			const string& _className, HICON _hIconSm,  const int &_cbWndExtra, const int &_cbClsExtra = 0);
		~Class(void);

		bool reg(void);

	};
}
#endif
#endif