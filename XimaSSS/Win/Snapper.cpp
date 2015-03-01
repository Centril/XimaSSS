#include "..\StdAfx.hpp"

#include "Snapper.hpp"

#include "OptionsHandler.hpp"
#include "../Globals.func.hpp"
#include "Dialog.class.hpp"

#include <Cximage/ximacfg.h>
#include <Cximage/ximage.h> 

#include "../../XCaptureHelper/XCaptureHelper.h"

std::vector<std::string> explode(const std::string &inString, const std::string &separator)
{
	std::vector<std::string> returnVector;
	std::string::size_type start = 0;
	std::string::size_type end = 0;

	while((end=inString.find(separator, start)) != std::string::npos)
	{
		returnVector.push_back (inString.substr (start, end-start));
		start = end + separator.size();
	}

	returnVector.push_back(inString.substr(start));
	return returnVector;
}

namespace Win
{
	__inline void unitToPixel(int &height, int &width, const float &_width, const float &_height, const string &_unit)
	{
		if(_unit == "pixels")
		{
			height = static_cast<int>(_height);
			width = static_cast<int>(_width);
		}
		else if(_unit == "cm")
		{
			height = round(_height * 28.3553875);
			width = round(_width * 28.3553875);
		}
		else if(_unit == "mm")
		{
			height = round(_height * 2.83473495);
			width = round(_width * 2.83473495);
		}
		else if(_unit == "inches")
		{
			height = round(_height * 71.99424);
			width = round(_width * 71.99424);
		}
		else
			throw std::runtime_error("illegal unit");
	}

	__inline string typeToSuffix(const string &_type)
	{
		static map<string, string> _check;
		_check["Joint Photographic Experts Group | JPEG"] = ".jpg";
		_check["Joint Photographic Experts Group | JPEG 2000"] = ".jp2";
		_check["Joint Bi-level Image Experts Group | JBIG"] = ".jbg";
		_check["Graphics Interchange Format | GIF"] = ".gif";
		_check["Portable Network Graphics | PNG"] = ".png";
		_check["Bitmap | BMP"] = ".bmp";
		_check["Portable Any Map Graphic Bitmap | PNM"] = ".pnm";
		_check["Tagged Image File Format | TIFF"] = ".tif";
		_check["Truevision TARGA | TGA "] = ".tga";
		_check["Paint Shop Pro Compressed Graphic"] = ".jas";
		_check["Sun Raster Graphic | RAS"] = ".ras";
		_check["PCX"] = ".pcx";
		return _check[_type];
	}

	__inline string translateVariables(const string &_filename)
	{
		vector<string> _vars;
		vector<string> _nonvars;
		{
			vector<string> _parts = explode(_filename, "%");
			for(int i = 0; i < (int)_parts.size(); i++)
				if(i % 2)
					_vars.push_back(_parts[i]);
				else
					_nonvars.push_back(_parts[i]);
		}
		{
			time_t rawtime;
			time(&rawtime);
			tm* timeinfo;
			localtime(&rawtime);
			static regex randomMinToMax("^random (\\d+) to (\\d+)$", regex_constants::perl);
			static regex randomWithMax("^random (\\d+)$", regex_constants::perl);

			for(int i = 0; i < (int)_vars.size(); i++)
			{
				string &_var = _vars[i];

				/* year litterals */
				if(_var == "Y")
				{
					char buffer[5];
					strftime(buffer, sizeof buffer, "%Y", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "y")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%y", timeinfo);
					_var = buffer;
					continue;
				}
				/* month litterals */
				else if(_var == "F")
				{
					char buffer[10];
					strftime(buffer, sizeof buffer, "%B", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "M")
				{
					char buffer[4];
					strftime(buffer, sizeof buffer, "%b", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "m")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%m", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "n")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%m", timeinfo);
					if(buffer[0] == '0')
					{
						_var = buffer;
						_var = _var.substr(1);
					}
					else
						_var = buffer;
					continue;
				}
				/*day litterals */
				else if(_var == "d")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%d", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "j")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%d", timeinfo);
					if(buffer[0] == '0')
						_var = buffer[1];
					else
						_var = buffer;
					continue;
				}
				else if(_var == "D")
				{
					char buffer[4];
					strftime(buffer, sizeof buffer, "%a", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "L")
				{
					char buffer[10];
					strftime(buffer, sizeof buffer, "%A", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "z")
				{
					char buffer[4];
					strftime(buffer, sizeof buffer, "%j", timeinfo);
					if(buffer[0] == '0' && buffer[1] == '0')
					{
						_var = buffer;
						_var = _var.substr(2);
					}
					else if(buffer[0] == '0')
					{
						_var = buffer;
						_var = _var.substr(1);
					}
					else
						_var = buffer;
					continue;
				}
				else if(_var == "Z")
				{
					char buffer[4];
					strftime(buffer, sizeof buffer, "%j", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "w")
				{
					char buffer[2];
					strftime(buffer, sizeof buffer, "%w", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "O")
				{
					int n;
					{
						char buffer[3];
						strftime(buffer, sizeof buffer, "%d", timeinfo);
						if(buffer[0] == '0')
						{
							_var = buffer;
							_var = _var.substr(1);
						}
						else
							_var = buffer;
					}
					n = stoi(_var);
					if(n == 1 || n == 21 || n == 31)
						_var = "st";
					else if(n == 2 || n == 22)
						_var = "nd";
					else if(n == 3 || n == 23)
						_var = "rd";
					else
						_var = "th";

					continue;
				}
				/* time litterals */
				else if(_var == "a")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%A", timeinfo);
					_var = buffer[0] == 'A' ? "am" : "pm";
					continue;
				}
				else if(_var == "A")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%p", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "g")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%I", timeinfo);
					if(buffer[0] == '0')
					{
						_var = buffer;
						_var = _var.substr(1);
					}
					else
						_var = buffer;
					continue;
				}
				else if(_var == "G")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%H", timeinfo);
					if(buffer[0] == '0')
					{
						_var = buffer;
						_var = _var.substr(1);
					}
					else
						_var = buffer;
					continue;
				}
				else if(_var == "h")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%I", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "H")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%H", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "i")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%M", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "I")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%M", timeinfo);
					if(buffer[0] == '0')
					{
						_var = buffer;
						_var = _var.substr(1);
					}
					else
						_var = buffer;
					continue;
				}
				else if(_var == "s")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%S", timeinfo);
					_var = buffer;
					continue;
				}
				else if(_var == "S")
				{
					char buffer[3];
					strftime(buffer, sizeof buffer, "%S", timeinfo);
					if(buffer[0] == '0')
					{
						_var = buffer;
						_var = _var.substr(1);
					}
					else
						_var = buffer;
					continue;
				}

				boost::smatch what;
				if(regex_match(_var, what, randomMinToMax))
				{
					int min = stoi(what[1]), range = (stoi(what[2]) - min) + 1;
					_var = itos(rand() % range + min);
				}
				else if(regex_match(_var, what, randomWithMax))
				{
					int top = stoi(what[1]);
					if(top + 1 > RAND_MAX)
						_var = itos(rand());
					else
						_var = itos(rand() % top + 1);
				}
			}
		}

		string filename;
		{
			int size = (int)_nonvars.size() - 1;
			for(int i = 0; i < size; i++)
				filename += _nonvars[i] + _vars[i];
			filename += _nonvars[size];
		}

		return filename;
	}

	__inline string generateIncrement(const string &_program)
	{
		unsigned long id;
		{
			HKEY hkey;
			RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\XimaSSS", 0, 0, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, 0, &hkey,0);
			unsigned long val;
			{
				unsigned long valSize = sizeof val;
				RegQueryValueEx(hkey, _program.c_str(), 0, 0, (LPBYTE) &val, &valSize);
				if(val > 99)
					val = 0;
			}
			RegCloseKey(hkey);
			id = val;
			val++;
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\XimaSSS", 0, KEY_SET_VALUE, &hkey);
			RegSetValueEx(hkey, _program.c_str(), 0, REG_DWORD, (const unsigned char*) &val, sizeof DWORD);
			RegCloseKey(hkey);
		}
		return itos(id);
	}

	__inline string toLower(const string &str)
	{
		string lc;
		for(int i = 0; i < (int)str.size(); i++)
			lc += static_cast<char>(tolower(str[i]));
		return lc;
	}

	__inline bool addToList(string _path, const string &_ild, const string &_filename)
	{
		try // SECTION: Make directory if it doesn't exists
		{
			filesystem::path boostPath(_path, filesystem::native);
			if( (!filesystem::exists(boostPath)) || (!filesystem::is_directory(boostPath)) )
				if(!filesystem::create_directory(boostPath))
					return false;
		}
		catch(filesystem::filesystem_error&)
		{
			return false;
		}

		ofstream iList((_path + _ild + ".ild").c_str(), ios_base::app);
		if(!iList.is_open())
			return false;
		iList.write((_filename + "\n").c_str(), _filename.size() + 1);
		iList.close();
		return true;
	}

	__inline void drawDate(HDC &_dc, const Options::Date &_date)
	{
		if(!_date.enable)
			return;

		HFONT font = CreateFont
		(
			-MulDiv(_date.fontSize, GetDeviceCaps(_dc, LOGPIXELSY), 72),
			0, 0, 0,
			getWeight(_date.fontStyle), isItalic(_date.fontStyle), 0, 0, ANSI_CHARSET, 0, 0, 0, 0,
			_date.fontName.c_str()
		);

		SetTextColor(_dc, RGB(_date.fontColor[0], _date.fontColor[1], _date.fontColor[2]));
		SetBkMode(_dc, TRANSPARENT);
		HGDIOBJ old = SelectObject(_dc, font);

		string text = translateVariables(_date.textformat);
		TextOut(_dc, _date.xPos, _date.yPos, text.c_str(), text.size());
		DeleteObject(font);
		SelectObject(_dc, old);
	}

	__inline bool saveImage(CxImage &image, const string &filename, const string &_suffix, const Options::Image &img)
	{
		int type = 0;
		{
			if(_suffix == ".jpg")
				type = CXIMAGE_FORMAT_JPG;
			else if(_suffix == ".jp2")
				type = CXIMAGE_FORMAT_JP2;
			else if(_suffix == ".jbg")
				type = CXIMAGE_FORMAT_JBG;
			else if(_suffix == ".gif")
				type = CXIMAGE_FORMAT_GIF;
			else if(_suffix == ".png")
				type = CXIMAGE_FORMAT_PNG;
			else if(_suffix == ".bmp")
				type = CXIMAGE_FORMAT_BMP;
			else if(_suffix == ".tif")
				type = CXIMAGE_FORMAT_TIF;
			else if(_suffix == ".tga")
				type = CXIMAGE_FORMAT_TGA;
			else if(_suffix == ".pcx")
				type = CXIMAGE_FORMAT_PCX;
			else if(_suffix == ".jas")
				type = CXIMAGE_FORMAT_PGX;
			else if(_suffix == ".ras")
				type = CXIMAGE_FORMAT_RAS;
			else if(_suffix == ".pnm")
				type = CXIMAGE_FORMAT_PNM;
		}

		switch(type)
		{
		case CXIMAGE_FORMAT_JPG:
			image.SetJpegQuality(100);
			image.SetXDPI(img.resolution);
			image.SetYDPI(img.resolution);
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_JPG);
			break;
		case CXIMAGE_FORMAT_JP2:
			image.SetJpegQuality(100);
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_JP2);
			break;
		case CXIMAGE_FORMAT_JBG:
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_JBG);
			break;
		case CXIMAGE_FORMAT_GIF:
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_GIF);
			break;
		case CXIMAGE_FORMAT_PNG:
			image.SetXDPI(img.resolution);
			image.SetYDPI(img.resolution);
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_PNG);
			break;
		case CXIMAGE_FORMAT_BMP:
			image.SetXDPI(img.resolution);
			image.SetYDPI(img.resolution);
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_BMP);
			break;
		case CXIMAGE_FORMAT_TIF:
			image.SetXDPI(img.resolution);
			image.SetYDPI(img.resolution);
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_TIF);
			break;
		case CXIMAGE_FORMAT_TGA:
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_TGA);
			break;
		case CXIMAGE_FORMAT_PCX:
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_PCX);
			break;
		case CXIMAGE_FORMAT_PGX:
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_PGX);
			break;
		case CXIMAGE_FORMAT_RAS:
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_RAS);
			break;
		case CXIMAGE_FORMAT_PNM:
			return image.Save(filename.c_str(), CXIMAGE_FORMAT_PNM);
			break;
		default:
			return false;
		}
	}

	Snapper::Snapper(Options &_o, Window& _w) : options(_o), m_refMainWindow(_w)
	{
	}

	Snapper& Snapper::operator =(const Snapper &_rhs)
	{
		if(this == &_rhs)
			return *this;
		this->m_refMainWindow = _rhs.m_refMainWindow;
		return *this;
	}

	Snapper::~Snapper(void)
	{
	}

	__inline int _stdcall EnumMonitorCallback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, long dwData)
	{
		UNREFERENCED_PARAMETER(hdcMonitor); UNREFERENCED_PARAMETER(hMonitor);
		LPRECT rct = (LPRECT)dwData;
		UnionRect(rct, rct, lprcMonitor);
		return 1;
	}

	__inline void GetVirtualScreenRect(LPRECT inRct)
	{
		RECT rct={0,0,0,0};
		EnumDisplayMonitors(0, 0, EnumMonitorCallback, (long)&rct);
		CopyRect(inRct, &rct);
	}

	void Snapper::multimonitor(void)
	{
		test("m");
		SM_CXVIRTUALSCREEN;
		SM_CYVIRTUALSCREEN;
	}

	__inline bool capturePrimary(const string &filename, const string &suffix,
		const Options::Date &date, const Options::Image &img)
	{
		if(img.resize)
		{
			int width = GetSystemMetrics(SM_CXSCREEN);
			int height = GetSystemMetrics(SM_CYSCREEN);
			HDC hScreen = GetDC(0); // get the screen dc
			if(!hScreen)
			{
				ReleaseDC(0, hScreen);
				return false;
			}
			HBITMAP hScreenImage = CreateCompatibleBitmap(hScreen, width, height);
			if(!hScreenImage)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				return false;
			}
			HDC hMemdc = CreateCompatibleDC(0); // create a working memory dc object
			if(!hMemdc)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}
			HBITMAP hOldImage = (HBITMAP)SelectObject(hMemdc, hScreenImage);
			if(!BitBlt(hMemdc, 0, 0, width, height, hScreen, 0, 0, SRCCOPY))
			{
				SelectObject(hMemdc, hOldImage);
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}

			if(!(date.xPos >= width || date.yPos >= height))
				drawDate(hMemdc, date);

			SelectObject(hMemdc, hOldImage);
			DeleteDC(hMemdc);
			ReleaseDC(0, hScreen);

			CxImage image;
			if(!image.CreateFromHBITMAP(hScreenImage))
				return false;
			DeleteObject(hScreenImage);

			int _width, _height;
			unitToPixel(_height, _width, img.width, img.height, img.unit);
			if(_width <= width)
				width = _width;
			if(_height <= height)
				height = _height;

			if(!image.Resample(width, height, 2, 0))
				return false;
			if(!image.IncreaseBpp(24))
				return false;
			if(!saveImage(image, filename, suffix, img))
				return false;
		}
		else
		{
			int height, width; // convert unit to pixels (or return if already in pixels)
			unitToPixel(height, width, img.width, img.height, img.unit);
			{
				int _width = GetSystemMetrics(SM_CXSCREEN);
				int _height = GetSystemMetrics(SM_CYSCREEN);
				if(width > _width)
					width = _width;
				if(height > _height)
					height = _height;
			}
			HDC hScreen = GetDC(0); // get the screen dc
			if(!hScreen)
			{
				ReleaseDC(0, hScreen);
				return false;
			}
			HBITMAP hScreenImage = CreateCompatibleBitmap(hScreen, width, height);
			if(!hScreenImage)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				return false;
			}
			HDC hMemdc = CreateCompatibleDC(0); // create a working memory dc object
			if(!hMemdc)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}
			HBITMAP hOldImage = (HBITMAP)SelectObject(hMemdc, hScreenImage);
			if(!BitBlt(hMemdc, 0, 0, width, height, hScreen, 0, 0, SRCCOPY))
			{
				SelectObject(hMemdc, hOldImage);
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}

			if(!(date.xPos >= width || date.yPos >= height))
				drawDate(hMemdc, date);

			SelectObject(hMemdc, hOldImage);
			DeleteDC(hMemdc);
			ReleaseDC(0, hScreen);

			CxImage image;
			if(!image.CreateFromHBITMAP(hScreenImage))
				return false;

			DeleteObject(hScreenImage);

			if(!saveImage(image, filename, suffix, img))
				return false;
		}
		return true;
	}

	void Snapper::primary(void)
	{
		string application = "";
		{ // SECTION: Determine what SpecialApplication's Options (if any) that should be used
			string processName = toLower(Window::getForeground().getFilename());
			if(processName != "")
			{
				map<string, Options::SpecialProgram>::const_iterator iter;
				for(iter = options.specialPrograms.begin(); iter != options.specialPrograms.end(); iter++)
				{
					if(toLower(iter->second.executablePath) == processName)
					{
						application = iter->first;
						break;
					}
				}
			}
		}

		if(application == "") // if its the general settings.
		{
			string filename = options.filenameFormat; // translate everything and make a valid filename
			string suffix = typeToSuffix(options.image.fileType);
			filename = "general" + translateVariables(filename) + "." + generateIncrement("~GENERAL~") + suffix;

			string path = options.path;
			if(!(path[path.size() - 1] == '/' || path[path.size() - 1] == '\\'))
				path += "/";

			if(!addToList(path, "~GENERAL~", filename)) // add the image to the Image List database.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
			if(!capturePrimary(path + filename, suffix, options.date, options.image)) // capture primary;
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
		}
		else // its a special application.
		{
			const Options::SpecialProgram &app = options.specialPrograms.find(application)->second;
			string nativeappname;
			{ // SECTION: Remove illegal characters (for native[NTFS Win32]).
				string::const_iterator iter;
				for(iter = app.name.begin(); iter != app.name.end(); iter++)
				{
					if(*iter == '|' || *iter == '\\' || *iter == '?' || *iter == '*' || *iter == '<' || *iter == '"' ||
						*iter == ':' || *iter == '>' || *iter == '/')
						continue;
					nativeappname += *iter;
				}
			}

			string filename = app.filenameFormat; // translate everything and make a valid filename
			string suffix = typeToSuffix(app.image.fileType);
			filename = nativeappname + translateVariables(filename) + "." + generateIncrement(nativeappname) + suffix;

			string path = app.path;
			if(!(path[path.size() - 1] == '/' || path[path.size() - 1] == '\\'))
				path += "/";

			if(!addToList(path, nativeappname, filename)) // add the image to the Image List database.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
			if(!capturePrimary(path + filename, suffix, app.date, app.image)) // capture primary;
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
		}
	}

	static HINSTANCE hCaptureHelper;
	void Snapper::abortWindow(void)
	{
		if(!EndWindowSelection())
		{
			Dialog::messageBox(this->m_refMainWindow, "Couldn't unhook the selection procedure.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}
		if(!FreeLibrary(hCaptureHelper))
		{
			Dialog::messageBox(this->m_refMainWindow, "DLL Unload Failed.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}
	}

	__inline bool captureWindow(Window& _window, const string &filename, const string &suffix,
		const Options::Date &date, const Options::Image &img)
	{
		if(img.resize)
		{
			int width, height; // convert unit to pixels (or return if already in pixels)
			int x, y;
			{
				// Get window width & height
				RECT rect;
				_window.getRect(&rect);
				x = 0;
				y = 0;
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
			}
			HDC hScreen = GetWindowDC(_window); // get the screen dc
			if(!hScreen)
			{
				ReleaseDC(0, hScreen);
				return false;
			}
			HBITMAP hScreenImage = CreateCompatibleBitmap(hScreen, width, height);
			if(!hScreenImage)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				return false;
			}
			HDC hMemdc = CreateCompatibleDC(0); // create a working memory dc object
			if(!hMemdc)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}
			HBITMAP hOldImage = (HBITMAP)SelectObject(hMemdc, hScreenImage);
			if(!BitBlt(hMemdc, 0, 0, width, height, hScreen, x, y, SRCCOPY))
			{
				SelectObject(hMemdc, hOldImage);
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}

			if(!(date.xPos >= width || date.yPos >= height))
				drawDate(hMemdc, date);

			SelectObject(hMemdc, hOldImage);
			DeleteDC(hMemdc);
			ReleaseDC(0, hScreen);

			CxImage image;
			if(!image.CreateFromHBITMAP(hScreenImage))
				return false;
			DeleteObject(hScreenImage);

			int _width, _height;
			unitToPixel(_height, _width, img.width, img.height, img.unit);
			if(_width <= width)
				width = _width;
			if(_height <= height)
				height = _height;

			if(!image.Resample(width, height, 2, 0))
				return false;
			if(!image.IncreaseBpp(24))
				return false;
			if(!saveImage(image, filename, suffix, img))
				return false;
		}
		else
		{
			int width, height; // convert unit to pixels (or return if already in pixels)
			int x, y;
			unitToPixel(height, width, img.width, img.height, img.unit);
			{
				RECT rect;
				_window.getRect(&rect);
				x = 0;
				y = 0;
				{
					int _width = rect.right - rect.left;
					int _height = rect.bottom - rect.top;
					if(width > _width)
						width = _width;
					if(height > _height)
						height = _height;
				}
			}
			HDC hScreen = GetWindowDC(_window); // get the screen dc
			if(!hScreen)
			{
				ReleaseDC(0, hScreen);
				return false;
			}
			HBITMAP hScreenImage = CreateCompatibleBitmap(hScreen, width, height);
			if(!hScreenImage)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				return false;
			}
			HDC hMemdc = CreateCompatibleDC(0); // create a working memory dc object
			if(!hMemdc)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}
			HBITMAP hOldImage = (HBITMAP)SelectObject(hMemdc, hScreenImage);
			if(!BitBlt(hMemdc, 0, 0, width, height, hScreen, x, y, SRCCOPY))
			{
				SelectObject(hMemdc, hOldImage);
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}

			if(!(date.xPos >= width || date.yPos >= height))
				drawDate(hMemdc, date);

			SelectObject(hMemdc, hOldImage);
			DeleteDC(hMemdc);
			ReleaseDC(0, hScreen);

			CxImage image;
			if(!image.CreateFromHBITMAP(hScreenImage))
				return false;

			DeleteObject(hScreenImage);

			if(!saveImage(image, filename, suffix, img))
				return false;
		}
		return true;
	}

	void Snapper::window(void)
	{
		HWND _hwnd = EndWindowSelection();
		if(!_hwnd)
		{
			Dialog::messageBox(this->m_refMainWindow, "Couldn't unhook the selection procedure.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}
		if(!FreeLibrary(hCaptureHelper))
		{
			Dialog::messageBox(this->m_refMainWindow, "DLL Unload Failed.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}

		Window w(_hwnd);
		string application = "";
		{ // SECTION: Determine what SpecialApplication's Options (if any) that should be used
		  // Window has to "be foreground"!
			POINT pt;
			GetCursorPos(&pt);
			if(Window::getForeground().containsPoint(pt.x, pt.y))
			{
				string processName = toLower(w.getFilename());
				if(processName != "")
				{
					map<string, Options::SpecialProgram>::const_iterator iter;
					for(iter = options.specialPrograms.begin(); iter != options.specialPrograms.end(); iter++)
					{
						if(toLower(iter->second.executablePath) == processName)
						{
							application = iter->first;
							break;
						}
					}
				}
			}
		}

		if(application == "") // if its the general settings.
		{
			string filename = options.filenameFormat; // translate everything and make a valid filename
			string suffix = typeToSuffix(options.image.fileType);
			filename = "general" + translateVariables(filename) + "." + generateIncrement("~GENERAL~") + suffix;

			string path = options.path;
			if(!(path[path.size() - 1] == '/' || path[path.size() - 1] == '\\'))
				path += "/";

			if(!addToList(path, "~GENERAL~", filename)) // add the image to the Image List database.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the window.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
			if(!captureWindow(w, path + filename, suffix, options.date, options.image)) // capture primary;
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the window.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
		}
		else // its a special application.
		{
			const Options::SpecialProgram &app = options.specialPrograms.find(application)->second;
			string nativeappname;
			{ // SECTION: Remove illegal characters (for native[NTFS Win32]).
				string::const_iterator iter;
				for(iter = app.name.begin(); iter != app.name.end(); iter++)
				{
					if(*iter == '|' || *iter == '\\' || *iter == '?' || *iter == '*' || *iter == '<' || *iter == '"' ||
						*iter == ':' || *iter == '>' || *iter == '/')
						continue;
					nativeappname += *iter;
				}
			}

			string filename = app.filenameFormat; // translate everything and make a valid filename
			string suffix = typeToSuffix(app.image.fileType);
			filename = nativeappname + translateVariables(filename) + "." + generateIncrement(nativeappname) + suffix;

			string path = app.path;
			if(!(path[path.size() - 1] == '/' || path[path.size() - 1] == '\\'))
				path += "/";

			if(!addToList(path, nativeappname, filename)) // add the image to the Image List database.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
			if(!captureWindow(w, path + filename, suffix, app.date, app.image)) // capture primary;
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
		}
	}

	void Snapper::selectWindow(bool &flag)
	{
		flag = true;

		hCaptureHelper = LoadLibrary((const char*)(filesystem::initial_path().string() + "/XCaptureHelper.dll").c_str());
		if(!hCaptureHelper)
		{
			Dialog::messageBox(this->m_refMainWindow, "DLL Load Failed.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}
		if(!BeginWindowSelection())
		{
			Dialog::messageBox(this->m_refMainWindow, "Hook Load Failed.", "Capture Failed", MB_ICONEXCLAMATION);
			FreeLibrary(hCaptureHelper);
			return;
		}
	}

	__inline bool captureForeground(const string &filename, const string &suffix,
		const Options::Date &date, const Options::Image &img)
	{
		if(img.resize)
		{
			int width, height; // convert unit to pixels (or return if already in pixels)
			int x, y;
			unitToPixel(height, width, img.width, img.height, img.unit);
			Window _w = Window::getForeground();
			{
				// Get foreground area width & height
				RECT rectFG;
				_w.getRect(&rectFG);
				x = rectFG.left;
				y = rectFG.top;
				width = rectFG.right - rectFG.left;
				height = rectFG.bottom - rectFG.top;
			}
			HDC hScreen = GetWindowDC(_w); // get the foregroundwindow dc
			if(!hScreen)
			{
				ReleaseDC(_w, hScreen);
				return false;
			}
			HBITMAP hScreenImage = CreateCompatibleBitmap(hScreen, width, height);
			if(!hScreenImage)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(_w, hScreen);
				return false;
			}
			HDC hMemdc = CreateCompatibleDC(0); // create a working memory dc object
			if(!hMemdc)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(_w, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}
			HBITMAP hOldImage = (HBITMAP)SelectObject(hMemdc, hScreenImage);
			if(!BitBlt(hMemdc, 0, 0, width, height, hScreen, x, y, SRCCOPY))
			{
				SelectObject(hMemdc, hOldImage);
				DeleteObject(hScreenImage);
				ReleaseDC(_w, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}

			if(!(date.xPos >= width || date.yPos >= height))
				drawDate(hMemdc, date);

			SelectObject(hMemdc, hOldImage);
			DeleteDC(hMemdc);
			ReleaseDC(_w, hScreen);

			CxImage image;
			if(!image.CreateFromHBITMAP(hScreenImage))
				return false;
			DeleteObject(hScreenImage);

			int _width, _height;
			unitToPixel(_height, _width, img.width, img.height, img.unit);
			if(_width <= width)
				width = _width;
			if(_height <= height)
				height = _height;

			if(!image.Resample(width, height, 2, 0))
				return false;
			if(!image.IncreaseBpp(24))
				return false;
			if(!saveImage(image, filename, suffix, img))
				return false;
		}
		else
		{
			int width, height; // convert unit to pixels (or return if already in pixels)
			int x, y;
			unitToPixel(height, width, img.width, img.height, img.unit);
			Window _w = Window::getForeground();
			{
				// Get foreground area width & height
				RECT rectFG;
				_w.getRect(&rectFG);
				x = rectFG.left;
				y = rectFG.top;
				{
					int _width = rectFG.right - rectFG.left;
					int _height = rectFG.bottom - rectFG.top;
					if(width > _width)
						width = _width;
					if(height > _height)
						height = _height;
				}
			}
			HDC hScreen = GetWindowDC(_w); // get the foreground window dc
			if(!hScreen)
			{
				ReleaseDC(_w, hScreen);
				return false;
			}
			HBITMAP hScreenImage = CreateCompatibleBitmap(hScreen, width, height);
			if(!hScreenImage)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(_w, hScreen);
				return false;
			}
			HDC hMemdc = CreateCompatibleDC(0); // create a working memory dc object
			if(!hMemdc)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(_w, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}
			HBITMAP hOldImage = (HBITMAP)SelectObject(hMemdc, hScreenImage);
			if(!BitBlt(hMemdc, 0, 0, width, height, hScreen, x, y, SRCCOPY))
			{
				SelectObject(hMemdc, hOldImage);
				DeleteObject(hScreenImage);
				ReleaseDC(_w, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}

			if(!(date.xPos >= width || date.yPos >= height))
				drawDate(hMemdc, date);

			SelectObject(hMemdc, hOldImage);
			DeleteDC(hMemdc);
			ReleaseDC(_w, hScreen);

			CxImage image;
			if(!image.CreateFromHBITMAP(hScreenImage))
				return false;

			DeleteObject(hScreenImage);

			if(!saveImage(image, filename, suffix, img))
				return false;
		}
		return true;
	}

	void Snapper::foreground(void)
	{
		string application = "";
		{ // SECTION: Determine what SpecialApplication's Options (if any) that should be used
			string processName = toLower(Window::getForeground().getFilename());
			if(processName != "")
			{
				map<string, Options::SpecialProgram>::const_iterator iter;
				for(iter = options.specialPrograms.begin(); iter != options.specialPrograms.end(); iter++)
				{
					if(toLower(iter->second.executablePath) == processName)
					{
						application = iter->first;
						break;
					}
				}
			}
		}

		if(application == "") // if its the general settings.
		{
			string filename = options.filenameFormat; // translate everything and make a valid filename
			string suffix = typeToSuffix(options.image.fileType);
			filename = "general" + translateVariables(filename) + "." + generateIncrement("~GENERAL~") + suffix;

			string path = options.path;
			if(!(path[path.size() - 1] == '/' || path[path.size() - 1] == '\\'))
				path += "/";

			if(!addToList(path, "~GENERAL~", filename)) // add the image to the Image List database.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
			if(!captureForeground(path + filename, suffix, options.date, options.image)) // capture FG.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
		}
		else // its a special application.
		{
			const Options::SpecialProgram &app = options.specialPrograms.find(application)->second;
			string nativeappname;
			{ // SECTION: Remove illegal characters (for native[NTFS Win32]).
				string::const_iterator iter;
				for(iter = app.name.begin(); iter != app.name.end(); iter++)
				{
					if(*iter == '|' || *iter == '\\' || *iter == '?' || *iter == '*' || *iter == '<' || *iter == '"' ||
						*iter == ':' || *iter == '>' || *iter == '/')
						continue;
					nativeappname += *iter;
				}
			}

			string filename = app.filenameFormat; // translate everything and make a valid filename
			string suffix = typeToSuffix(app.image.fileType);
			filename = nativeappname + translateVariables(filename) + "." + generateIncrement(nativeappname) + suffix;

			string path = app.path;
			if(!(path[path.size() - 1] == '/' || path[path.size() - 1] == '\\'))
				path += "/";

			if(!addToList(path, nativeappname, filename)) // add the image to the Image List database.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the foreground area.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
			if(!captureForeground(path + filename, suffix, app.date, app.image)) // capture FG.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the foreground area.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
		}
	}

	void Snapper::abortArea(void)
	{
		RECT _dummy;
		if(!EndAreaSelection(_dummy))
		{
			Dialog::messageBox(this->m_refMainWindow, "Couldn't unhook the selection procedure.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}
		if(!FreeLibrary(hCaptureHelper))
		{
			Dialog::messageBox(this->m_refMainWindow, "DLL Unload Failed.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}
	}

	void Snapper::selectArea(bool &flag)
	{
		flag = true;

		hCaptureHelper = LoadLibrary((const char*)(filesystem::initial_path().string() + "/XCaptureHelper.dll").c_str());
		if(!hCaptureHelper)
		{
			Dialog::messageBox(this->m_refMainWindow, "DLL Load Failed.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}
		if(!BeginAreaSelection())
		{
			Dialog::messageBox(this->m_refMainWindow, "Hook Load Failed.", "Capture Failed", MB_ICONEXCLAMATION);
			FreeLibrary(hCaptureHelper);
			return;
		}
	}

	void Snapper::area(void)
	{
		RECT _bounds;
		if(!EndAreaSelection(_bounds))
		{
			Dialog::messageBox(this->m_refMainWindow, "Couldn't unhook the selection procedure.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}
		if(!FreeLibrary(hCaptureHelper))
		{
			Dialog::messageBox(this->m_refMainWindow, "DLL Unload Failed.", "Capture Failed", MB_ICONEXCLAMATION);
			return;
		}
	}

	__inline bool captureWorkingArea(const string &filename, const string &suffix,
		const Options::Date &date, const Options::Image &img)
	{
		if(img.resize)
		{
			int width, height; // convert unit to pixels (or return if already in pixels)
			int x, y;
			{
				// Get workingarea width & height
				RECT rectWD;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWD, 0);
				x = rectWD.left;
				y = rectWD.top;
				width = rectWD.right - rectWD.left;
				height = rectWD.bottom - rectWD.top;
			}
			HDC hScreen = GetDC(0); // get the screen dc
			if(!hScreen)
			{
				ReleaseDC(0, hScreen);
				return false;
			}
			HBITMAP hScreenImage = CreateCompatibleBitmap(hScreen, width, height);
			if(!hScreenImage)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				return false;
			}
			HDC hMemdc = CreateCompatibleDC(0); // create a working memory dc object
			if(!hMemdc)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}
			HBITMAP hOldImage = (HBITMAP)SelectObject(hMemdc, hScreenImage);
			if(!BitBlt(hMemdc, 0, 0, width, height, hScreen, x, y, SRCCOPY))
			{
				SelectObject(hMemdc, hOldImage);
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}

			if(!(date.xPos >= width || date.yPos >= height))
				drawDate(hMemdc, date);

			SelectObject(hMemdc, hOldImage);
			DeleteDC(hMemdc);
			ReleaseDC(0, hScreen);

			CxImage image;
			if(!image.CreateFromHBITMAP(hScreenImage))
				return false;
			DeleteObject(hScreenImage);

			int _width, _height;
			unitToPixel(_height, _width, img.width, img.height, img.unit);
			if(_width <= width)
				width = _width;
			if(_height <= height)
				height = _height;

			if(!image.Resample(width, height, 2, 0))
				return false;
			if(!image.IncreaseBpp(24))
				return false;
			if(!saveImage(image, filename, suffix, img))
				return false;
		}
		else
		{
			int width, height; // convert unit to pixels (or return if already in pixels)
			int x, y;
			unitToPixel(height, width, img.width, img.height, img.unit);
			{
				// Get workingarea width & height
				RECT rectWD;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWD, 0);
				x = rectWD.left;
				y = rectWD.top;
				{
					int _width = rectWD.right - rectWD.left;
					int _height = rectWD.bottom - rectWD.top;
					if(width > _width)
						width = _width;
					if(height > _height)
						height = _height;
				}
			}
			HDC hScreen = GetDC(0); // get the screen dc
			if(!hScreen)
			{
				ReleaseDC(0, hScreen);
				return false;
			}
			HBITMAP hScreenImage = CreateCompatibleBitmap(hScreen, width, height);
			if(!hScreenImage)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				return false;
			}
			HDC hMemdc = CreateCompatibleDC(0); // create a working memory dc object
			if(!hMemdc)
			{
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}
			HBITMAP hOldImage = (HBITMAP)SelectObject(hMemdc, hScreenImage);
			if(!BitBlt(hMemdc, 0, 0, width, height, hScreen, x, y, SRCCOPY))
			{
				SelectObject(hMemdc, hOldImage);
				DeleteObject(hScreenImage);
				ReleaseDC(0, hScreen);
				ReleaseDC(0, hMemdc);
				return false;
			}

			if(!(date.xPos >= width || date.yPos >= height))
				drawDate(hMemdc, date);

			SelectObject(hMemdc, hOldImage);
			DeleteDC(hMemdc);
			ReleaseDC(0, hScreen);

			CxImage image;
			if(!image.CreateFromHBITMAP(hScreenImage))
				return false;

			DeleteObject(hScreenImage);

			if(!saveImage(image, filename, suffix, img))
				return false;
		}
		return true;
	}

	void Snapper::workingarea(void)
	{
		string application = "";
		{ // SECTION: Determine what SpecialApplication's Options (if any) that should be used
			string processName = toLower(Window::getForeground().getFilename());
			if(processName != "")
			{
				map<string, Options::SpecialProgram>::const_iterator iter;
				for(iter = options.specialPrograms.begin(); iter != options.specialPrograms.end(); iter++)
				{
					if(toLower(iter->second.executablePath) == processName)
					{
						application = iter->first;
						break;
					}
				}
			}
		}

		if(application == "") // if its the general settings.
		{
			string filename = options.filenameFormat; // translate everything and make a valid filename
			string suffix = typeToSuffix(options.image.fileType);
			filename = "general" + translateVariables(filename) + "." + generateIncrement("~GENERAL~") + suffix;

			string path = options.path;
			if(!(path[path.size() - 1] == '/' || path[path.size() - 1] == '\\'))
				path += "/";

			if(!addToList(path, "~GENERAL~", filename)) // add the image to the Image List database.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
			if(!captureWorkingArea(path + filename, suffix, options.date, options.image)) // capture WD.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the primary screen.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
		}
		else // its a special application.
		{
			const Options::SpecialProgram &app = options.specialPrograms.find(application)->second;
			string nativeappname;
			{ // SECTION: Remove illegal characters (for native[NTFS Win32]).
				string::const_iterator iter;
				for(iter = app.name.begin(); iter != app.name.end(); iter++)
				{
					if(*iter == '|' || *iter == '\\' || *iter == '?' || *iter == '*' || *iter == '<' || *iter == '"' ||
						*iter == ':' || *iter == '>' || *iter == '/')
						continue;
					nativeappname += *iter;
				}
			}

			string filename = app.filenameFormat; // translate everything and make a valid filename
			string suffix = typeToSuffix(app.image.fileType);
			filename = nativeappname + translateVariables(filename) + "." + generateIncrement(nativeappname) + suffix;

			string path = app.path;
			if(!(path[path.size() - 1] == '/' || path[path.size() - 1] == '\\'))
				path += "/";

			if(!addToList(path, nativeappname, filename)) // add the image to the Image List database.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the workingarea.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
			if(!captureWorkingArea(path + filename, suffix, app.date, app.image)) // capture WD.
			{
				Dialog::messageBox(Window::getDesktop(), "Failed to capture the workingarea.", "Capture Failed", MB_ICONEXCLAMATION);
				return;
			}
		}
	}
}