#ifndef _IMAGEMENUITEM_HPP // Inclution guard
#define _IMAGEMENUITEM_HPP

#ifdef WIN32
#include "CGdiPlusBitmap.h"
#include <tchar.h>

template<class CallbackClass>
class ImageMenuItem
{
public:
	int m_xStart, m_yStart;
	int m_nWidth, m_nHeight;
	int m_imageLocation;

	typedef void (CallbackClass::*actionPtr)(void);
	actionPtr m_clickMethodPtr;
	actionPtr m_hoverMethodPtr;
	actionPtr m_leaveMethodPtr;
	CallbackClass& m_callbackObject;

public:
	ImageMenuItem(const int &_imageLocation, const int &_xStart, const int &_yStart, const int &_nWidth, const int &_nHeight,
		actionPtr _clickMethodPtr, actionPtr _hoverMethodPtr, actionPtr _leaveMethodPtr, CallbackClass& _callbackObject)
		: m_xStart(_xStart), m_yStart(_yStart), m_nWidth(_nWidth), m_nHeight(_nHeight), m_imageLocation(_imageLocation),
		m_clickMethodPtr(_clickMethodPtr), m_hoverMethodPtr(_hoverMethodPtr), m_leaveMethodPtr(_leaveMethodPtr),
		m_callbackObject(_callbackObject)
	{
	}

	ImageMenuItem<CallbackClass>& operator=(const ImageMenuItem<CallbackClass>& _rhs)
	{
		if(this == &_rhs)
		{
			return *this;
		}

		this->m_xStart = _rhs.m_xStart;
		this->m_yStart = _rhs.m_yStart;
		this->m_nWidth = _rhs.m_nWidth;
		this->m_nHeight = _rhs.m_nHeight;

		this->m_imageLocation = _rhs.m_imageLocation;

		this->m_callbackObject = _rhs.m_callbackObject;
		this->m_clickMethodPtr = _rhs.m_clickMethodPtr;
		this->m_hoverMethodPtr = _rhs.m_hoverMethodPtr;
		this->m_leaveMethodPtr = _rhs.m_leaveMethodPtr;

		return *this;
	}

	void paint(Graphics &_gfx)
	{
		CGdiPlusBitmapResource img;
		if(img.load(this->m_imageLocation, _T("PNG")))
			_gfx.DrawImage(img, this->m_xStart, this->m_yStart, this->m_nWidth, this->m_nHeight);
	}

	void onClick(Graphics &_gfx, Color &_color)
	{
		SolidBrush _brush(_color);
		_gfx.FillRectangle(&_brush, this->m_xStart, this->m_yStart, this->m_nWidth, this->m_nHeight);
		this->paint(_gfx);

		if(this->m_clickMethodPtr != 0)
			(this->m_callbackObject.*this->m_clickMethodPtr)();
	}

	void onHover(Graphics &_gfx, Color &_color)
	{
		SolidBrush _brush(_color);
		_gfx.FillRectangle(&_brush, this->m_xStart, this->m_yStart, this->m_nWidth, this->m_nHeight);
		this->paint(_gfx);

		if(this->m_hoverMethodPtr != 0)
			(this->m_callbackObject.*this->m_hoverMethodPtr)();
	}

	void onLeave(Graphics &_gfx, Color &_color, const bool &_call = true)
	{
		SolidBrush _brush(_color);
		_gfx.FillRectangle(&_brush, this->m_xStart, this->m_yStart, this->m_nWidth, this->m_nHeight);
		this->paint(_gfx);

		if(this->m_leaveMethodPtr != 0 && _call)
			(this->m_callbackObject.*this->m_leaveMethodPtr)();
	}

	bool hasBounds(const int &xPos, const int &yPos)
	{
		return (xPos >= this->m_xStart) && (xPos <= (this->m_xStart + this->m_nWidth)) && (yPos >= this->m_yStart) && (yPos <= (this->m_yStart + this->m_nHeight));
	}

	~ImageMenuItem(void)
	{
	}
};
#endif
#endif