#ifdef WIN32

#include "..\StdAfx.hpp"

#include "Class.class.hpp"

namespace Win
{
	Class::Class(void)
	{
	}

	Class::Class(WNDPROC _winProc, HINSTANCE _hInst, HICON _hIcon, HBRUSH _hBG,
		const string& _className, HICON _hIconSm,  const int &_cbWndExtra, const int &_cbClsExtra)
	{
		this->m_class.cbSize		= sizeof(WNDCLASSEX);
		this->m_class.lpfnWndProc	= _winProc;
		this->m_class.hInstance		= _hInst;
		this->m_class.lpszClassName	= _className.c_str();
		this->m_class.hIcon			= _hIcon;
		this->m_class.hIconSm		= _hIconSm;
		this->m_class.hbrBackground	= _hBG;
		this->m_class.style			= 0;
		this->m_class.cbClsExtra	= _cbClsExtra;
		this->m_class.cbWndExtra	= _cbWndExtra;
		this->m_class.hCursor		= LoadCursor(0, IDC_ARROW);
		this->m_class.lpszMenuName	= 0;
	}

	Class::~Class(void)
	{
	}

	bool Class::reg(void)
	{
		if(!RegisterClassEx(&this->m_class))
			return false;
		else return true;
	}
}
#endif