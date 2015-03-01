#ifndef _IMAGEMENUVERTICAL_HPP // Inclution guard
#define _IMAGEMENUVERTICAL_HPP

#ifdef WIN32
#include "ImageMenuItem.hpp"

#define TOP 0
#define CENTER 1
#define BOTTOM 2
#define MAXOFFSET 3
#define JUSTIFY 4

namespace Win
{
	template<class RefCallbackClass> // ImageMenuItem needs to know the class to find the method pointers in
	class ImageMenuVertical
	{
	private:
		std::vector< ImageMenuItem<RefCallbackClass> > m_items; // std::vector: O(1)->random access, O(1)->insertions at back.
		int m_indexOfClickedItem, m_indexOfHoveredItem;

		int m_xStart, m_yStart;
		int m_nWidth, m_nHeight;
		int m_offset, m_perImage;

		Color m_bgColor, m_clickColor, m_hoverColor;

	public:
		ImageMenuVertical(void)
		{
		}

		ImageMenuVertical(const int &_xStart, const int &_yStart, const int &_nWidth, const int &_nHeight, const int &_perImage, const int &_offset) :
			m_indexOfClickedItem(-1), m_indexOfHoveredItem(-1),
			m_xStart(_xStart), m_yStart(_yStart), m_nWidth(_nWidth), m_nHeight(_nHeight),
			m_offset(_offset), m_perImage(_perImage)
		{
		}

		ImageMenuVertical& operator=(const ImageMenuVertical& _rhs)
		{
			if(this == &_rhs)
				return *this;

			this->m_bgColor = _rhs.m_bgColor;
			this->m_clickColor = _rhs.m_clickColor;
			this->m_hoverColor = _rhs.m_hoverColor;
			this->m_indexOfClickedItem = _rhs.m_indexOfClickedItem;
			this->m_indexOfHoveredItem = _rhs.m_indexOfHoveredItem;
			this->m_items = _rhs.m_items;
			this->m_nHeight = _rhs.m_nHeight;
			this->m_nWidth = _rhs.m_nWidth;
			this->m_offset = _rhs.m_offset;
			this->m_perImage = _rhs.m_perImage;
			this->m_xStart = _rhs.m_xStart;
			this->m_yStart = _rhs.m_yStart;

			return *this;
		}

		void setDefault(const int &_index)
		{
			try
			{
				this->m_items.at(_index);
				this->m_indexOfClickedItem = _index;
			}
			catch(std::out_of_range)
			{
				throw;
			}
		}

		void initiate(const Color &_bgColor, const Color &_clickColor, const Color &_hoverColor)
		{
			this->m_bgColor = _bgColor;
			this->m_clickColor = _clickColor;
			this->m_hoverColor = _hoverColor;
		}

		void insert(const int &_imageLocation, RefCallbackClass& _obj, void (RefCallbackClass::*_clickMethodPtr)(void),
			void (RefCallbackClass::*_hoverMethodPtr)(void), void (RefCallbackClass::*_leaveMethodPtr)(void))
		{
			if(!this->m_items.empty())
			{
				int _yStart = this->m_items.back().m_yStart + this->m_perImage + this->m_offset;
				this->m_items.push_back(ImageMenuItem<RefCallbackClass>(_imageLocation, this->m_xStart, _yStart, this->m_nWidth, this->m_perImage, _clickMethodPtr, _hoverMethodPtr, _leaveMethodPtr, _obj));
			}
			else
				this->m_items.push_back(ImageMenuItem<RefCallbackClass>(_imageLocation, this->m_xStart, this->m_yStart, this->m_nWidth, this->m_perImage, _clickMethodPtr, _hoverMethodPtr, _leaveMethodPtr, _obj));
		}

		void align(const int &_direction) // aligns the menu to LEFT/CENTER/RIGHT
		{
			if(_direction == TOP) // aligning to left
			{
				std::vector< ImageMenuItem<RefCallbackClass> > temp_items;
				for(unsigned int _i = 0; _i < this->m_items.size(); _i++)
				{
					ImageMenuItem<RefCallbackClass> &_item = this->m_items[_i];
					if(_i == 0)
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, this->m_yStart, this->m_nWidth, this->m_perImage, _item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					else
					{
						int _yStart = temp_items[_i - 1].m_yStart + this->m_perImage + this->m_offset;
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, _yStart, this->m_nWidth, this->m_perImage, _item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					}
				}
				this->m_items.assign(temp_items.begin(), temp_items.end());
			}
			else if(_direction == BOTTOM) // aligning to right
			{
				// find where the menu starts
				int _realYStart = 0;
				for(unsigned int _i = 0; _i < this->m_items.size(); _i++)
				{
					if(_i == 0)
						_realYStart = this->m_perImage;
					else
						_realYStart += (this->m_perImage + this->m_offset);
				}
				_realYStart = this->m_nHeight - _realYStart;

				std::vector< ImageMenuItem<RefCallbackClass> > temp_items;
				for(unsigned int _i = 0; _i < this->m_items.size(); _i++)
				{
					ImageMenuItem<RefCallbackClass> &_item = this->m_items[_i];
					if(_i == 0)
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, _realYStart + this->m_yStart, this->m_nWidth, this->m_perImage, _item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					else
					{
						int _yStart = _realYStart + this->m_items[_i - 1].m_yStart + this->m_perImage + this->m_offset;
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, _yStart, this->m_nWidth, this->m_perImage, _item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					}
				}
				this->m_items.assign(temp_items.begin(), temp_items.end());
			}
			else if(_direction == CENTER) // aligning to center
			{
				int _realYStart = 0;
				for(unsigned int _i = 0; _i < this->m_items.size(); _i++)
				{
					if(_i == 0)
						_realYStart = this->m_perImage;
					else
						_realYStart += (this->m_perImage + this->m_offset);
				}
				_realYStart =  round((this->m_nHeight / 2) - (_realYStart / 2));

				std::vector< ImageMenuItem<RefCallbackClass> > temp_items;
				for(unsigned int _i = 0; _i < this->m_items.size(); _i++)
				{
					ImageMenuItem<RefCallbackClass> &_item = this->m_items[_i];
					if(_i == 0)
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, _realYStart + this->m_yStart, this->m_nWidth, this->m_perImage, _item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					else
					{
						int _yStart = _realYStart + this->m_items[_i - 1].m_yStart + this->m_perImage + this->m_offset;
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, _yStart, this->m_nWidth, this->m_perImage, _item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					}
				}
				this->m_items.assign(temp_items.begin(), temp_items.end());
			}
			else if(_direction == MAXOFFSET)
			{
				int _offsetPerItem = round( (this->m_nHeight - this->m_items.size() * this->m_perImage) / (this->m_items.size() - 1) );

				std::vector< ImageMenuItem<RefCallbackClass> > temp_items;
				for(unsigned int _i = 0; _i < this->m_items.size(); _i++)
				{
					ImageMenuItem<RefCallbackClass> &_item = this->m_items[_i];
					if(_i == 0)
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, this->m_yStart, this->m_nWidth, this->m_perImage, _item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					else
					{
						int _yStart = temp_items[_i - 1].m_yStart + this->m_perImage + _offsetPerItem;
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, _yStart, this->m_nWidth, this->m_perImage, _item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					}
				}
				this->m_items.assign(temp_items.begin(), temp_items.end());
			}
			else if(_direction == JUSTIFY)
			{
				int _offsetPerItem = round( (this->m_nHeight - this->m_items.size() * this->m_perImage) / (this->m_items.size() + 1) );

				std::vector< ImageMenuItem<RefCallbackClass> > temp_items;
				for(unsigned int _i = 0; _i < this->m_items.size(); _i++)
				{
					ImageMenuItem<RefCallbackClass> &_item = this->m_items[_i];
					if(_i == 0)
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, this->m_yStart + _offsetPerItem, this->m_nWidth, this->m_perImage,_item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					else
					{
						int _yStart = temp_items[_i - 1].m_yStart + this->m_perImage + _offsetPerItem;
						temp_items.push_back(ImageMenuItem<RefCallbackClass>(_item.m_imageLocation, this->m_xStart, _yStart, this->m_nWidth, this->m_perImage, _item.m_clickMethodPtr, _item.m_hoverMethodPtr, _item.m_leaveMethodPtr, _item.m_callbackObject));
					}
				}
				this->m_items.assign(temp_items.begin(), temp_items.end());
			}
			else
				return;
		}

		void reClickActive(Graphics &_gfx)
		{
			// Reclick the current clicked item if any
			if((this->m_indexOfClickedItem > -1))
			{
				try
				{
					this->m_items.at(this->m_indexOfClickedItem).onClick(_gfx, this->m_clickColor);
				}
				catch(std::out_of_range)
				{
					return;
				}
			}
		}

		// Paint all the menu items (Usualy used when the window is repainted)
		void paint(Graphics &_gfx)
		{
			// Paint the background
			SolidBrush _brush(this->m_bgColor);
			_gfx.FillRectangle(&_brush, this->m_xStart, this->m_yStart, this->m_nWidth, this->m_nHeight);

			if(this->m_items.empty())
				return;

			this->reClickActive(_gfx);

			// Paint the current hovered item if any
			if((this->m_indexOfHoveredItem > -1))
			{			
				try
				{
					this->m_items.at(this->m_indexOfHoveredItem).onHover(_gfx, this->m_hoverColor);
				}
				catch(std::out_of_range)
				{
					return;
				}
			}

			// Paint the other items
			for(int _index = 0; _index < static_cast<int>(this->m_items.size()); _index++)
			{
				if(this->m_indexOfClickedItem != _index && this->m_indexOfHoveredItem != _index)
					this->m_items[_index].paint(_gfx);
			}
		}

		// Check if the position is within the menu's bounds
		bool hasBounds(const int &xPos, const int &yPos)
		{
			if(this->m_items.empty())
				return false;

			if( ((xPos >= this->m_xStart) && (xPos <= this->m_nWidth + this->m_xStart)) && ((yPos >= this->m_yStart) && (yPos <= this->m_nHeight + this->m_yStart)) )
				return true;

			return false;
		}

		// Route a click to the correct item
		bool routeClick(Graphics &_gfx, const int &xPos, const int &yPos)
		{
			// Check if the position isn't within the menu
			if(!this->hasBounds(xPos, yPos))
				return false;

			// iterate thru all items
			for(int _index = 0; _index < static_cast<int>(this->m_items.size()); _index++)
			{
				// check if the current item received a click action
				ImageMenuItem<RefCallbackClass> &_item(this->m_items.at(_index));
				if((this->m_indexOfClickedItem != _index) && _item.hasBounds(xPos, yPos))
				{
					if(this->m_indexOfClickedItem != -1) // Make the previously clicked item(if any) lose focus
						// only one menuitem can have focus at one time so leave the previous
						this->m_items.at(this->m_indexOfClickedItem).onLeave(_gfx, this->m_bgColor);

					// route the click to the item
					this->m_indexOfClickedItem = _index;
					_item.onClick(_gfx, this->m_clickColor);

					// An item can't be hovered and clicked at the same time
					if(this->m_indexOfClickedItem == this->m_indexOfHoveredItem)
						this->m_indexOfHoveredItem = -1;
					return true;
				}
				else
					continue;
			}
			return false;
		}

		// Route a hover to the correct item
		void routeHover(Graphics &_gfx, const int &xPos, const int &yPos)
		{
			// iterate thru all items
			for(int _index = 0; _index < static_cast<int>(this->m_items.size()); _index++)
			{
				if(this->m_indexOfHoveredItem == _index || this->m_indexOfClickedItem == _index)
					continue;

				// check if the current item received a hover action
				ImageMenuItem<RefCallbackClass> &_item = this->m_items.at(_index);
				if(_item.hasBounds(xPos, yPos))
				{
					// route the hover to the item
					_item.onHover(_gfx, this->m_hoverColor);
					this->m_indexOfHoveredItem = _index;
					return;
				}
				else // check if the next one... since this one wasn't
					continue;
			}
		}

		// Checks if the current hovered item has been left
		bool isHoveredItemLeft(const int &xPos, const int &yPos)
		{
			// check if there are no items, if no items there can't be any item left
			if(this->m_items.empty())
				return false;
			// check if no item is hovered,  if no item hovered there can't be any item left
			if(this->m_indexOfHoveredItem == -1)
				return false;

			// check if the item is left
			ImageMenuItem<RefCallbackClass> &_item(this->m_items.at(this->m_indexOfHoveredItem));
			if(!_item.hasBounds(xPos, yPos))
				return true;

			return false;
		}

		// Route a leave to the hovered item(if any)
		void routeLeave(Graphics &_gfx)
		{
			try
			{
				this->m_items.at(this->m_indexOfHoveredItem).onLeave(_gfx, this->m_bgColor, false);
				this->m_indexOfHoveredItem = -1;
			}
			catch(std::out_of_range)
			{
				return;
			}
		}

		void tab(Graphics &_gfx)
		{
			if(this->m_indexOfClickedItem != -1)
			{
				this->m_items.at(this->m_indexOfClickedItem).onLeave(_gfx, this->m_bgColor);
				this->m_indexOfHoveredItem = -1;
			}

			this->m_indexOfClickedItem++;
			if(m_indexOfClickedItem >= (int)this->m_items.size())
				this->m_indexOfClickedItem = 0;
			this->m_items.at(this->m_indexOfClickedItem).onClick(_gfx, this->m_clickColor);
		}

		~ImageMenuVertical(void)
		{
		}
	};
}
#endif
#endif