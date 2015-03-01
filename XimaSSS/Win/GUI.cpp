#ifdef WIN32

#include "..\StdAfx.hpp"

#include "Main.hpp"
#include "GUI.hpp"

#include "../Globals.func.hpp"
#include "XBrowseForFolder.hpp"
#include "../FTP/FTPUploader.class.hpp"

#include <commctrl.h>
#include <shlobj.h>
#include "makelink.h"

static Win::Options* g_opt = 0;

__inline int __stdcall generalproc(HWND _hWndDlg, unsigned int _message, unsigned int _wParam, long _lParam)
{
	UNREFERENCED_PARAMETER(_lParam);
	using namespace Win;

	static Win::Dialog _d;

	switch(_message)
	{
	case WM_INITDIALOG:
	{
		_d = Win::Dialog(_hWndDlg);
		_d.setItemText(IDC_PATH, g_opt->path);
		_d.setItemText(IDC_FORMAT, g_opt->filenameFormat);
		char buff[260];
		SHGetSpecialFolderPath(0, buff, 0x0007, 0);
		if(filesystem::exists(filesystem::path(buff + string("\\XimaSSS.lnk"), filesystem::native)))
			CheckDlgButton(_hWndDlg, IDC_AUTOSTART, 1);

		return 1;
		break;
	}
	case WM_COMMAND:
	{
		if(HIWORD(_wParam) == BN_CLICKED && LOWORD(_wParam) == IDC_BROWSE)
		{
			char buffer[512];
			if(XBrowseForFolder(_d, "C:\\", buffer, 512))
				_d.setItemText(IDC_PATH, buffer);
		}
		break;
	}
	case GUI_SAVE:
	{
		Options* opt = reinterpret_cast<Options*>(_wParam);

		if(_d.getItemText(IDC_PATH) == "")
			return 1;
		if(_d.getItemText(IDC_FORMAT) == "")
			return 1;

		try
		{
			filesystem::exists(filesystem::path(_d.getItemText(IDC_PATH), filesystem::native));
		}
		catch(filesystem::filesystem_error)
		{
			return 1;
		}

		regex expression("[<>|\"/?\\\\*:]", regex_constants::perl);
		if(regex_search(_d.getItemText(IDC_FORMAT), expression))
			return 1;

		opt->path = _d.getItemText(IDC_PATH);
		opt->filenameFormat = _d.getItemText(IDC_FORMAT);
		break;
	}
	default:
		return 0;
		break;
    } 
    return 0;
}

__inline int __stdcall imageproc(HWND _hWndDlg, unsigned int _message, unsigned int _wParam, long _lParam)
{
	using namespace Win;
	static map<HWND, string> prevWidthUnit, prevHeightUnit;

	Win::Dialog _d(_hWndDlg);

	switch(_message)
	{
	case WM_INITDIALOG:
	{
		Win::Options::Image* img;
		if(_lParam == -1)
			img = &g_opt->image;
		else
			if(g_opt->specialPrograms.find((char*)_lParam) != g_opt->specialPrograms.end())
				img = &g_opt->specialPrograms[(char*)_lParam].image;
			else
				img = &g_opt->image;

		_d.setItemText(IDC_FILETYPE, img->fileType);
		CheckDlgButton(_hWndDlg, IDC_RESIZE, (int)img->resize);
		_d.setItemText(IDC_DISPLAYQUALITY, itos(img->quality));
		_d.setItemText(IDC_WIDTH, ftos(img->width));
		_d.setItemText(IDC_WIDTHUNIT, img->unit);
		_d.setItemText(IDC_HEIGHT, ftos(img->height));
		_d.setItemText(IDC_HEIGHTUNIT, img->unit);
		_d.setItemText(IDC_RESOLUTION, itos(img->resolution));

		_d.setItemText(IDC_DISPLAYQUALITY, itos(img->quality));
		_d.getItem(IDC_QUALITY).sendMessage(WM_USER + 6, 1, MAKELONG(1, 100));
		_d.getItem(IDC_QUALITY).sendMessage(WM_USER + 5, 1, img->quality);

		const char* _filetypes[] =
			{
				"Joint Photographic Experts Group | JPEG",
				"Joint Photographic Experts Group | JPEG 2000",
				"Joint Bi-level Image Experts Group | JBIG",
				"Graphics Interchange Format | GIF",
				"Portable Network Graphics | PNG",
				"Bitmap | BMP",
				"Portable Any Map Graphic Bitmap | PNM",
				"Tagged Image File Format | TIFF",
				"Truevision TARGA | TGA",
				"Paint Shop Pro Compressed Graphic",
				"Sun Raster Graphic | RAS",
				"PCX",
			};
		const char* _units[] = { "pixels", "inches", "cm", "mm" };

		for(int _i = 0; _i < 12; _i++)
			_d.getItem(IDC_FILETYPE).sendMessage(CB_ADDSTRING, 0, reinterpret_cast<long>( (LPCTSTR)_filetypes[_i]));

		for(int _i = 0; _i < 4; _i++)
		{
			_d.getItem(IDC_WIDTHUNIT).sendMessage(CB_ADDSTRING, 0, reinterpret_cast<long>( (LPCTSTR)_units[_i]));
			_d.getItem(IDC_HEIGHTUNIT).sendMessage(CB_ADDSTRING, 0, reinterpret_cast<long>( (LPCTSTR)_units[_i]));
		}

		prevWidthUnit[_hWndDlg] = img->unit;
		prevHeightUnit[_hWndDlg] = img->unit;

		return 1;
		break;
	}
		break;
	case WM_COMMAND:
		if(HIWORD(_wParam) == CBN_CLOSEUP && (LOWORD(_wParam) == IDC_HEIGHTUNIT || LOWORD(_wParam) == IDC_WIDTHUNIT))
		{
			const char* _units[] = { "pixels", "inches", "cm", "mm" };
			double _cF;
			string _text(_units[_d.getItem(LOWORD(_wParam)).sendMessage(CB_GETCURSEL, 0, 0)]);

			if(_text == prevWidthUnit[_hWndDlg])
				break;
			if(_text == "pixels" && prevWidthUnit[_hWndDlg] == "cm")
				_cF = 28.3553875;
			else if(_text == "pixels" && prevWidthUnit[_hWndDlg] == "mm")
				_cF = 2.83473495;
			else if(_text == "pixels" && prevWidthUnit[_hWndDlg] == "inches")
				_cF = 71.99424;
			else if(_text == "cm" && prevWidthUnit[_hWndDlg] == "pixels")
				_cF = 0.03526667;
			else if(_text == "cm" && prevWidthUnit[_hWndDlg] == "mm")
				_cF = 0.1;
			else if(_text == "cm" && prevWidthUnit[_hWndDlg] == "inches")
				_cF = 2.5389969;
			else if(_text == "mm" && prevWidthUnit[_hWndDlg] == "pixels")
				_cF = 0.35276667;
			else if(_text == "mm" && prevWidthUnit[_hWndDlg] == "cm")
				_cF = 10;
			else if(_text == "mm" && prevWidthUnit[_hWndDlg] == "inches")
				_cF = 25.389969;
			else if(_text == "inches" && prevWidthUnit[_hWndDlg] == "pixels")
				_cF = 0.01389;
			else if(_text == "inches" && prevWidthUnit[_hWndDlg] == "cm")
				_cF = 0.3938564;
			else if(_text == "inches" && prevWidthUnit[_hWndDlg] == "mm")
				_cF = 0.03938564;
			else
				break;

			_d.setItemText(IDC_HEIGHT, ftos(round( (float)(_cF * stof(_d.getItemText(IDC_HEIGHT))), _text == "pixels" ? 0 : 3)));
			_d.setItemText(IDC_WIDTH, ftos(round( (float)(_cF * stof(_d.getItemText(IDC_WIDTH))), _text == "pixels" ? 0 : 3)));
			_d.setItemText(LOWORD(_wParam) == IDC_HEIGHTUNIT ? IDC_WIDTHUNIT : IDC_HEIGHTUNIT, _text);
			prevWidthUnit[_hWndDlg] = prevHeightUnit[_hWndDlg] = _text;
		}
		break;
	case WM_HSCROLL:
		_d.setItemText(IDC_DISPLAYQUALITY, itos(_d.getItem(IDC_QUALITY).sendMessage(WM_USER, 0, 0)));
		break;
	case GUI_SAVE:
	{
		Options *opt = reinterpret_cast<Options*>(_wParam);
		Win::Options::Image* img;
		if(_lParam == -1)
			img = &opt->image;
		else
			img = &opt->specialPrograms[(char*)_lParam].image;

		regex expression("[^\\d.]", regex_constants::perl);
		if(regex_search(_d.getItemText(IDC_WIDTH), expression))
			return 1;
		if(regex_search(_d.getItemText(IDC_HEIGHT), expression))
			return 1;
		if(regex_search(_d.getItemText(IDC_RESOLUTION), expression))
			return 1;
		if(_d.getItemText(IDC_HEIGHTUNIT) != _d.getItemText(IDC_WIDTHUNIT))
			return 1;

		string _units[] = { "pixels", "inches", "cm", "mm" };
		const char* _filetypes[] =
			{
				"Joint Photographic Experts Group | JPEG",
				"Joint Photographic Experts Group | JPEG 2000",
				"Joint Bi-level Image Experts Group | JBIG",
				"Graphics Interchange Format | GIF",
				"Portable Network Graphics | PNG",
				"Bitmap | BMP",
				"Portable Any Map Graphic Bitmap | PNM",
				"Tagged Image File Format | TIFF",
				"Truevision TARGA | TGA",
				"Paint Shop Pro Compressed Graphic",
				"Sun Raster Graphic | RAS",
				"PCX",
			};

		bool success = false;
		for(int _i = 0; _i < 4; _i++)
			if(_units[_i] == _d.getItemText(IDC_HEIGHTUNIT))
				success = true;
		if(!success)
			return 1;

		success = false;
		for(int _i = 0; _i < 4; _i++)
			if(_units[_i] == _d.getItemText(IDC_WIDTHUNIT))
				success = true;
		if(!success)
			return 1;

		success = false;
		for(int _i = 0; _i < 12; _i++)
			if(_filetypes[_i] == _d.getItemText(IDC_FILETYPE))
				success = true;
		if(!success)
			return 1;

		img->fileType = _d.getItemText(IDC_FILETYPE);
		img->quality = stoi(_d.getItemText(IDC_DISPLAYQUALITY));
		img->width = stof(_d.getItemText(IDC_WIDTH));
		img->height = stof(_d.getItemText(IDC_HEIGHT));
		img->resolution = stoi(_d.getItemText(IDC_RESOLUTION));
		img->unit = _d.getItemText(IDC_WIDTHUNIT);
		img->resize = IsDlgButtonChecked(*_d, IDC_RESIZE) == BST_CHECKED ? true : false;
		break;
	}
	default:
		return 0;
		break;
    } 
    return 0;
}

struct __tConStruct
{
	HANDLE testThread, testDotter;
	bool fResult;
	int dots;
	HWND hFTC;

	__tConStruct() : testThread(0), testDotter(0), fResult(false), dots(0), hFTC(0)
	{
	}
	__tConStruct(HWND _hFTC) : testThread(0), testDotter(0), fResult(false), dots(0), hFTC(_hFTC)
	{
	}
};

map<HWND, __tConStruct> tCons;
unsigned long __stdcall ftpTestConn(void* a)
{
	HWND _id = reinterpret_cast<HWND>(a);
	__tConStruct &tCon = tCons[_id];

	tCon.fResult = false;
	Win::Dialog pD(tCon.hFTC);
	pD.setItemText(IDC_TESTCONN, "Abort");
	pD.setItemText(IDC_CONNSTAT, "Connecting");

	FTP::FTPUploader::initialize();
	FTP::FTPUploader ftpClient(pD.getItemText(IDC_Adress), pD.getItemText(IDC_USERNAME), pD.getItemText(IDC_PASSWORD), stoi(pD.getItemText(IDC_PORT)));

	if(ftpClient.login())
	{
		string _path = pD.getItemText(IDC_PATH);
		if(_path[0] != '/')
			_path = "/" + _path;

		if(ftpClient.changeDirectory(_path))
			pD.setItemText(IDC_CONNSTAT, "Connection succeeded");
		else
			pD.setItemText(IDC_CONNSTAT, "CWD Failed");
	}
	else
		pD.setItemText(IDC_CONNSTAT, "Connection failed");
	tCon.fResult = true;
	ftpClient.close();
	if(tCons.empty())
		FTP::FTPUploader::cleanup();
	pD.setItemText(IDC_TESTCONN, "Test Connection");
	tCon.hFTC = 0;
	tCon.testThread = 0;
	tCons.erase(_id);
	return 0;
}

__inline int __stdcall ftpproc(HWND _hWndDlg, unsigned int _message, unsigned int _wParam, long _lParam)
{
	using namespace std;
	Win::Dialog _d(_hWndDlg);

	switch(_message) 
	{
	case WM_INITDIALOG:
	{
		Win::Options::FTP* ftp;
		if(_lParam == -1 || _lParam == 0)
			ftp = &g_opt->ftp;
		else
			if(g_opt->specialPrograms.find((char*)_lParam) != g_opt->specialPrograms.end())
				ftp = &g_opt->specialPrograms[(char*)_lParam].ftp;
			else
				ftp = &g_opt->ftp;

		CheckDlgButton(*_d, IDC_FTPENABLE, (int)ftp->enable);
		CheckDlgButton(*_d, IDC_AUTOSEND, (int)ftp->autosend);
		_d.setItemText(IDC_Adress, ftp->server);
		_d.setItemText(IDC_PORT, itos(ftp->port));
		_d.setItemText(IDC_USERNAME, ftp->username);
		_d.setItemText(IDC_PASSWORD, ftp->password);
		_d.setItemText(IDC_PATH, ftp->path);

		return 1;
		break;
	}
	case WM_COMMAND:
		if(HIWORD(_wParam) == BN_CLICKED && LOWORD(_wParam) == IDC_TESTCONN)
		{
			if(_d.getItemText(IDC_TESTCONN) == "Abort")
			{
				__tConStruct &tCon = tCons[_d];
				TerminateThread(tCon.testThread, 0);
				_d.setItemText(IDC_TESTCONN, "Test Connection");
				_d.setItemText(IDC_CONNSTAT, "");
				tCons.erase(_d);
				if(tCons.empty())
					FTP::FTPUploader::cleanup();
			}
			else
			{
				__tConStruct tCon(_d);
				tCons[_d] = tCon;
				tCons[_d].testThread = CreateThread(0, 0, ftpTestConn, reinterpret_cast<void*>((HWND)_d), 0, 0);
			}
		}
		break;
	case GUI_SAVE:
	{
		Options *opt = reinterpret_cast<Options*>(_wParam);
		Win::Options::FTP* ftp;
		if(_lParam == -1)
			ftp = &opt->ftp;
		else
			ftp = &opt->specialPrograms[(char*)_lParam].ftp;

		ftp->enable = IsDlgButtonChecked(*_d, IDC_FTPENABLE) == BST_CHECKED ? true : false;
		ftp->autosend = IsDlgButtonChecked(*_d, IDC_AUTOSEND) == BST_CHECKED ? true : false;
		ftp->server = _d.getItemText(IDC_Adress);
		ftp->port = stoi(_d.getItemText(IDC_PORT));
		ftp->username = _d.getItemText(IDC_USERNAME);
		ftp->password = _d.getItemText(IDC_PASSWORD);
		ftp->path = _d.getItemText(IDC_FTPPATH);
		break;
	}
	default:
		return 0;
		break;
    } 
    return 0;
}

__inline int __stdcall dateproc(HWND _hWndDlg, unsigned int _message, unsigned int _wParam, long _lParam)
{
	using namespace Win;

	static map<HWND, CHOOSEFONT> _cfs;
	static map<HWND, LOGFONT> _fonts;

	static map<HWND, CHOOSECOLOR> _ccs;
	static COLORREF _acrCustClr[16];

	Win::Dialog _d(_hWndDlg);

	switch(_message) 
	{
	case WM_INITDIALOG:
	{
		Win::Options::Date* date;
		if(_lParam == -1 || _lParam == 0)
			date = &g_opt->date;
		else
			if(g_opt->specialPrograms.find((char*)_lParam) != g_opt->specialPrograms.end())
				date = &g_opt->specialPrograms[(char*)_lParam].date;
			else
				date = &g_opt->date;

		CheckDlgButton(*_d, IDC_DATEENABLE, (int)date->enable);
		_d.setItemText(IDC_XPOS, itos(date->xPos));
		_d.setItemText(IDC_YPOS, itos(date->yPos));
		_d.setItemText(IDC_DATEFORMAT, date->textformat);

		_d.setItemText(IDC_DISPLAYOPACITY, itos(date->opacity));
		_d.getItem(IDC_OPACITY).sendMessage(WM_USER + 6, 1, MAKELONG(1, 100));
		_d.getItem(IDC_OPACITY).sendMessage(WM_USER + 5, 1, date->opacity);

		LOGFONT _font;
		_font.lfCharSet = ANSI_CHARSET;
		_font.lfItalic = isItalic(date->fontStyle);
		_font.lfWeight = getWeight(date->fontStyle);
		strcpy(_font.lfFaceName, date->fontName.c_str());
		HDC hDC = GetDC(*_d);
		_font.lfHeight = -MulDiv(date->fontSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
		ReleaseDC(*_d, hDC);
		_fonts[_hWndDlg] = _font;

		CHOOSEFONT _cf;
		memset((&_cf), 0, sizeof _cf );
		_cf.lStructSize = sizeof _cf;
		_cf.hwndOwner = *_d;
		_cf.Flags = CF_SHOWHELP | CF_SCREENFONTS | CF_SELECTSCRIPT | CF_INITTOLOGFONTSTRUCT;
		_cf.lpLogFont = &_fonts[_hWndDlg];
		_cf.iPointSize = date->fontSize * 10;
		_cfs[_hWndDlg] = _cf;

		CHOOSECOLOR _cc;
		memset((&_cc), 0, sizeof _cc);
		_cc.lStructSize = sizeof _cc;
		_cc.hwndOwner = *_d;
		_cc.rgbResult = RGB(date->fontColor[0], date->fontColor[1], date->fontColor[2]);
		_cc.lpCustColors = (LPDWORD)_acrCustClr;
		_cc.Flags = CC_FULLOPEN | CC_ANYCOLOR | CC_RGBINIT;
		_ccs[_hWndDlg] = _cc;

		return 1;
		break;
	}
	case WM_COMMAND:
		if(HIWORD(_wParam) == BN_CLICKED && LOWORD(_wParam) == IDC_FONT)
			ChooseFont(&_cfs[_hWndDlg]);
		else if(HIWORD(_wParam) == BN_CLICKED && LOWORD(_wParam) == IDC_FONTCOLOR)
			ChooseColor(&_ccs[_hWndDlg]);
		break;
	case WM_HSCROLL:
	{
		int pos = _d.getItem(IDC_OPACITY).sendMessage(WM_USER, 0, 0);
		_d.setItemText(IDC_DISPLAYOPACITY, itos(pos));
		break;
	}
	case GUI_SAVE:
	{
		Options *opt = reinterpret_cast<Options*>(_wParam);
		Win::Options::Date* date;
		if(_lParam == -1 || _lParam == 0)
			date = &opt->date;
		else
			date = &opt->specialPrograms[(char*)_lParam].date;

		date->enable = IsDlgButtonChecked(*_d, IDC_DATEENABLE) == BST_CHECKED ? true : false;
		date->opacity = stoi(_d.getItemText(IDC_DISPLAYOPACITY));
		date->textformat = _d.getItemText(IDC_DATEFORMAT);
		date->xPos = stoi(_d.getItemText(IDC_XPOS));
		date->yPos = stoi(_d.getItemText(IDC_YPOS));

		date->fontColor[0] = GetRValue(_ccs[_hWndDlg].rgbResult);
		date->fontColor[1] = GetGValue(_ccs[_hWndDlg].rgbResult);
		date->fontColor[2] = GetBValue(_ccs[_hWndDlg].rgbResult);

		date->fontSize = _cfs[_hWndDlg].iPointSize / 10;
		date->fontStyle = makeStyle(_fonts[_hWndDlg].lfItalic, _fonts[_hWndDlg].lfWeight);
		date->fontName = _fonts[_hWndDlg].lfFaceName;
		break;
	}
	default:
		return 0;
		break;
    } 
    return 0;
}

struct tabInfo
{
private:
	Win::Dialog _currActive;
public:
	Win::Window _control;
	Win::Dialog _general, _image, _ftp, _date;

	tabInfo(void)
	{
	}

	~tabInfo(void)
	{
		_control.destroy();
	}

	void fix(void)
	{
		_currActive = Dialog(*_general);
	}

	void show(void)
	{
		_control.show();
		_control.update();
		_currActive.show();
	}

	void hide(void)
	{
		_control.hide();
		_currActive.hide();
	}

	void changeActive(void)
	{
		int i = TabCtrl_GetCurSel(*_control);
		_currActive.hide();
		if(i == 0)
			_currActive = Dialog(*_general);
		else if(i == 1)
			_currActive = Dialog(*_image);
		else if(i == 2)
			_currActive = Dialog(*_ftp);
		else if(i == 3)
			_currActive = Dialog(*_date);
		else
			throw std::out_of_range("Can only be 0(General), 1(Image), 2(FTP), 3(Date)");
		_currActive.show();
	}
};

__inline int __stdcall generalspproc(HWND _hWndDlg, unsigned int _message, unsigned int _wParam, long _lParam)
{
	using namespace std;
	Win::Dialog _d(_hWndDlg);

	switch(_message)
	{
	case WM_INITDIALOG:
	{
		if(g_opt->specialPrograms.find((char*)_lParam) != g_opt->specialPrograms.end())
		{
			Win::Options::SpecialProgram &general = g_opt->specialPrograms[(char*)_lParam];
			CheckDlgButton(*_d, IDC_DEFIMG, (int)general.useDefaultImage);
			CheckDlgButton(*_d, IDC_DEFDATE, (int)general.useDefaultDate);
			CheckDlgButton(*_d, IDC_DEFFTP, (int)general.useDefaultFTP);

			_d.setItemText(IDC_NAME, (char*)_lParam);
			_d.setItemText(IDC_PATH, general.path);
			_d.setItemText(IDC_EXPATH, general.executablePath);
			_d.setItemText(IDC_FORMAT, general.filenameFormat);
		}
		else
		{
			CheckDlgButton(*_d, IDC_DEFIMG, 0);
			CheckDlgButton(*_d, IDC_DEFDATE, 0);
			CheckDlgButton(*_d, IDC_DEFFTP, 0);

			_d.setItemText(IDC_NAME, (char*)_lParam);
			_d.setItemText(IDC_PATH, g_opt->path);
			_d.setItemText(IDC_FORMAT, g_opt->filenameFormat);
		}
		return 1;
		break;
	}
	case WM_COMMAND:
	{
		if(HIWORD(_wParam) == BN_CLICKED && LOWORD(_wParam) == IDC_BROWSE)
		{
			char buffer[512];
			if(XBrowseForFolder(_d, "C:\\", buffer, 512))
				_d.setItemText(IDC_PATH, buffer);
		}
		else if(HIWORD(_wParam) == BN_CLICKED && LOWORD(_wParam) == IDC_BROWSEX)
		{
			char buffer[256];
			OPENFILENAME _ofn;
			memset(&_ofn, 0, sizeof _ofn);
			_ofn.lStructSize = sizeof _ofn;
			_ofn.hwndOwner = _hWndDlg;
			_ofn.lpstrFile = buffer;
			_ofn.lpstrFile[0] = '\0';
			_ofn.nMaxFile = sizeof buffer;
			_ofn.lpstrFilter = "Executable\0*.EXE\0";
			_ofn.nFilterIndex = 1;
			_ofn.lpstrFileTitle = 0;
			_ofn.nMaxFileTitle = 0;
			_ofn.lpstrInitialDir = 0;
			_ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | 0x02000000;
			if(GetOpenFileName(&_ofn) == 1)
				_d.setItemText(IDC_EXPATH, buffer);
		}
		break;
	}
	case GUI_SAVE:
	{
		Options *opt = reinterpret_cast<Options*>(_wParam);
		Win::Options::SpecialProgram* general = &opt->specialPrograms[(char*)_lParam];

		if(_d.getItemText(IDC_PATH) == "")
			return 1;
		if(_d.getItemText(IDC_EXPATH) == "")
			return 1;
		if(_d.getItemText(IDC_FORMAT) == "")
			return 1;

		try
		{
			filesystem::exists(filesystem::path(_d.getItemText(IDC_PATH), filesystem::native));
			if(!filesystem::exists(filesystem::path(_d.getItemText(IDC_EXPATH), filesystem::native)))
				return -1;
		}
		catch(filesystem::filesystem_error)
		{
			return -1;
		}
		regex expression("[<>|\"/?\\\\*:]", regex_constants::perl);
		if(regex_search(_d.getItemText(IDC_FORMAT), expression))
			return -1;

		general->useDefaultImage = IsDlgButtonChecked(*_d, IDC_DEFIMG) == BST_CHECKED ? true : false;
		general->useDefaultFTP = IsDlgButtonChecked(*_d, IDC_DEFFTP) == BST_CHECKED ? true : false;
		general->useDefaultDate = IsDlgButtonChecked(*_d, IDC_DEFDATE) == BST_CHECKED ? true : false;
		general->name = (char*)_lParam;
		general->executablePath = _d.getItemText(IDC_EXPATH);
		general->path = _d.getItemText(IDC_PATH);
		general->filenameFormat = _d.getItemText(IDC_FORMAT);

		bool *_rets = new bool[3];
		_rets[0] = general->useDefaultImage;
		_rets[1] = general->useDefaultFTP;
		_rets[2] = general->useDefaultDate;
		return (int)_rets;
		break;
	}
	default:
		return 0;
		break;
    } 
    return 0;
}

__inline int __stdcall spproc(HWND _hWndDlg, unsigned int _message, unsigned int _wParam, long _lParam)
{
	using namespace Win;

	static tabInfo* _currProgram;
	static vector<tabInfo*> _programTabs;
	static vector<string> _programNames;
	static Win::Dialog _d;
	static Win::Window _tab;

	map<string, Win::Options::SpecialProgram> &sps = g_opt->specialPrograms;

	switch(_message)
	{
	case WM_INITDIALOG:
	{
		_d = Win::Dialog(_hWndDlg);
		_tab =  _d.getItem(IDC_SPS);

		char buffer[100];
		TCITEM tab;
		tab.mask = TCIF_TEXT | TCIF_IMAGE;
		tab.iImage = -1;
		tab.pszText = buffer;

		map<string, Options::SpecialProgram>::iterator iter;
		int count = 0;
		for(iter = sps.begin(); iter != sps.end(); iter++)
		{
			Options::SpecialProgram &_program = iter->second;
			tabInfo* _programTab = new tabInfo();

			strcpy(tab.pszText, _program.name.c_str());
			TabCtrl_InsertItem(*_tab, count, &tab);

			_programTab->_control.create
			(
				0, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				WC_TABCONTROL, "0",
				7, 32, 458, 278,
				0, *_tab
			);

			tab.mask = TCIF_TEXT | TCIF_IMAGE;
			strcpy(tab.pszText, "General");
			TabCtrl_InsertItem(_programTab->_control, 0, &tab);
			_programTab->_general.create(_programTab->_control, MAKEINTRESOURCE(IDD_GENERALSP), (DLGPROC) generalspproc, (long)_program.name.c_str());
			_programTab->_general.setPos(0, 5, 27, 449, 248);

			tab.mask = TCIF_TEXT | TCIF_IMAGE;
			strcpy(tab.pszText, "Image");
			TabCtrl_InsertItem(_programTab->_control, 1, &tab);
			_programTab->_image.create(_programTab->_control, MAKEINTRESOURCE(IDD_IMAGESP), (DLGPROC) imageproc, (long)_program.name.c_str());
			_programTab->_image.setPos(0, 5, 27, 449, 248);

			strcpy(tab.pszText, "FTP");
			TabCtrl_InsertItem(_programTab->_control, 2, &tab);
			_programTab->_ftp.create(_programTab->_control, MAKEINTRESOURCE(IDD_FTPSP), (DLGPROC) ftpproc, (long)_program.name.c_str());
			_programTab->_ftp.setPos(0, 5, 27, 449, 248);

			strcpy(tab.pszText, "Date");
			TabCtrl_InsertItem(_programTab->_control, 3, &tab);
			_programTab->_date.create(_programTab->_control, MAKEINTRESOURCE(IDD_DATESP), (DLGPROC) dateproc, (long)_program.name.c_str());
			_programTab->_date.setPos(0, 5, 27, 449, 248);

			_programTab->fix();
			if(count == 0)
			{
				_currProgram = _programTab;
				_currProgram->show();
			}
			_programTabs.push_back(_programTab);
			_programNames.push_back(_program.name);

			count++;
		}

		return 1;
		break;
	}
	case WM_COMMAND:
		if(HIWORD(_wParam) == BN_CLICKED && LOWORD(_wParam) == IDC_ADD)
		{
			string name = _d.getItemText(IDC_NAME);
			if(name == "") // NULL/"" == illegal && ~GENERAL~ IS RESERVED!!!
			{
				_d.setItemText(IDC_MESSAGE, "Invalid name");
				return 0;
			}
			else if(name == "~GENERAL~") // NULL/"" == illegal && ~GENERAL~ IS RESERVED!!!
			{
				_d.setItemText(IDC_MESSAGE, "The entry: ~GENERAL~ is reserved!");
				return 0;
			}

			if(binary_search(_programNames.begin(), _programNames.end(), name))
			{
				_d.setItemText(IDC_MESSAGE, "The entry: " + name + " already exists");
				return 0;
			}

			char buffer[100];
			int count = TabCtrl_GetItemCount(*_tab);
			TCITEM tab;
			tab.mask = TCIF_TEXT;
			tab.cchTextMax = 100;
			tab.pszText = buffer;
			strcpy(tab.pszText, name.c_str());
			TabCtrl_InsertItem(*_tab, count, &tab);
			tabInfo* _programTab = new tabInfo();
			_programTab->_control.create
			(
				0, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				WC_TABCONTROL, "0",
				7, 32, 458, 278,
				0, *_tab
			);

			tab.mask = TCIF_TEXT | TCIF_IMAGE;
			strcpy(tab.pszText, "General");
			TabCtrl_InsertItem(_programTab->_control, 0, &tab);
			_programTab->_general.create(_programTab->_control, MAKEINTRESOURCE(IDD_GENERALSP), (DLGPROC) generalspproc, (long)name.c_str());
			_programTab->_general.setPos(0, 5, 27, 449, 248);

			tab.mask = TCIF_TEXT | TCIF_IMAGE;
			strcpy(tab.pszText, "Image");
			TabCtrl_InsertItem(_programTab->_control, 1, &tab);
			_programTab->_image.create(_programTab->_control, MAKEINTRESOURCE(IDD_IMAGESP), (DLGPROC) imageproc, (long)name.c_str());
			_programTab->_image.setPos(0, 5, 27, 449, 248);

			strcpy(tab.pszText, "FTP");
			TabCtrl_InsertItem(_programTab->_control, 2, &tab);
			_programTab->_ftp.create(_programTab->_control, MAKEINTRESOURCE(IDD_FTPSP), (DLGPROC) ftpproc, (long)name.c_str());
			_programTab->_ftp.setPos(0, 5, 27, 449, 248);

			strcpy(tab.pszText, "Date");
			TabCtrl_InsertItem(_programTab->_control, 3, &tab);
			_programTab->_date.create(_programTab->_control, MAKEINTRESOURCE(IDD_DATESP), (DLGPROC) dateproc, (long)name.c_str());
			_programTab->_date.setPos(0, 5, 27, 449, 248);

			_programTab->fix();
			_programTabs.push_back(_programTab);
			_programNames.push_back(name);

			if(!_currProgram)
				_currProgram = _programTab;

			_currProgram->hide();
			_currProgram->show();

			_d.setItemText(IDC_NAME, "");
		}
		else if(HIWORD(_wParam) == BN_CLICKED && LOWORD(_wParam) == IDC_REMOVE)
		{
			char buffer[100];
			int _i = TabCtrl_GetCurSel(*_tab);
			if(_d.getItemText(IDC_NAME) == "" && !_programTabs.empty() && _i > -1)
			{
				TCITEM chTab;
				chTab.mask = TCIF_TEXT;
				chTab.cchTextMax = 100;
				chTab.pszText = buffer;
				TabCtrl_GetItem(*_tab, _i, &chTab);
				if(Win::Dialog::messageBox(*_d, string("Do you really want to remove the entry: ") + chTab.pszText + string(" ?"), "", MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					TabCtrl_DeleteItem(*_tab, _i);
					delete _programTabs.at(_i);
					_programTabs.erase(_programTabs.begin() + _i);
					_programNames.erase(_programNames.begin() + _i);
					if(TabCtrl_SetCurSel(*_tab, _i - 1) == -1)
						if(TabCtrl_SetCurSel(*_tab, _i) != -1)
						{
							TabCtrl_SetCurSel(*_tab, _i + 1);
						}
					if(!_programTabs.empty())
					{
						_currProgram = _programTabs.at(TabCtrl_GetCurSel(*_tab));
						_currProgram->show();
					}
					else
						_currProgram = 0;
				}
			}
			else if(_d.getItemText(IDC_NAME) != "" && !_programTabs.empty())
			{
				for(int _i = 0; _i < (int)_programNames.size(); _i++)
				{
					if(_d.getItemText(IDC_NAME) != _programNames[_i])
						continue;

					if(Win::Dialog::messageBox(*_d, "Do you really want to remove the entry: " + _d.getItemText(IDC_NAME) + " ?", "", MB_YESNO | MB_ICONQUESTION) != IDYES)
						return 0;

					delete _programTabs.at(_i);
					_programTabs.erase(_programTabs.begin() + _i);
					_programNames.erase(_programNames.begin() + _i);
					if(_i == TabCtrl_GetCurSel(*_tab))
					{
						if(TabCtrl_SetCurSel(*_tab, _i - 1) == -1)
							if(TabCtrl_SetCurSel(*_tab, _i) != -1)
							{
								TabCtrl_SetCurSel(*_tab, _i + 1);
							}
						
						if(!_programTabs.empty())
							_currProgram = _programTabs.at(TabCtrl_GetCurSel(*_tab));
						TabCtrl_DeleteItem(*_tab, _i);
					}
					else
					{
						TabCtrl_DeleteItem(*_tab, _i);
						_currProgram->hide();
					}
					if(_programTabs.empty())
						_currProgram = 0;
					if(!_programTabs.empty())
						_currProgram->show();
					return 0;
				}
				_d.setItemText(IDC_MESSAGE, "The entry: " + _d.getItemText(IDC_NAME) + " doesn't exist");
				return 0;
			}
			else 
				_d.setItemText(IDC_MESSAGE, "No item selected or no search phrase");
		}
		break;
	case WM_NOTIFY:
	{
		NMHDR &info = *(NMHDR*)_lParam; 
		if(info.code == TCN_SELCHANGE && info.idFrom == IDC_SPS)
		{
			if(_currProgram)
				_currProgram->hide();
			_currProgram = _programTabs.at(TabCtrl_GetCurSel(*_tab));
			_currProgram->show();
		}
		else if(info.code == TCN_SELCHANGE)
			_programTabs.at(TabCtrl_GetCurSel(*_tab))->changeActive();
		break;
	}
	case WM_DESTROY:
		for(unsigned int _i = 0; _i < _programTabs.size(); _i++)
			delete _programTabs.at(_i);
		_programTabs.clear();
		_programNames.clear();
		_currProgram = 0;
		_tab.destroy();
		break;
	case GUI_SAVE:
	{
		Options *opt = reinterpret_cast<Options*>(_wParam);
		for(int _i = 0; _i < (int)_programTabs.size(); _i++)
		{
			{
				if( _programNames.at(_i) == "~GENERAL~") // ~GENERAL~ IS RESERVED!!!
					return 1;
			}

			tabInfo &_info = *_programTabs.at(_i);
			const char* _name = _programNames.at(_i).c_str();
			int _ret = generalspproc(*_info._general, GUI_SAVE, _wParam, (long)_name);
			bool* _rets = (bool*)_ret;
			if(_ret == -1)
			{
				delete[] _rets;
				return 1;
			}

			if(_rets[0])
				opt->specialPrograms[_name].image = opt->image;
			else
				if(imageproc(*_info._image, GUI_SAVE, _wParam, (long)_name) == 1)
				{
					delete[] _rets;
					return 1;
				}
			if(_rets[1])
				opt->specialPrograms[_name].ftp = opt->ftp;
			else
				if(ftpproc(*_info._ftp, GUI_SAVE, _wParam, (long)_name) == 1)
				{
					delete[] _rets;
					return 1;
				}
			if(_rets[2])
				opt->specialPrograms[_name].date = opt->date;
			else
				if(dateproc(*_info._date, GUI_SAVE, _wParam, (long)_name) == 1)
				{
					delete[] _rets;
					return 1;
				}
			delete[] _rets;
		}
		break;
	}
	default:
		return 0;
		break;
    } 
    return 0;
}

namespace Win
{
	GUI::GUI(Win::Main &_refMain) : 
		m_refMain(_refMain), m_menu(Win::ImageMenuVertical<GUI>(5, 0, 50, 407, 50, 3)),
		m_window(), _general(), _image(), _ftp(), _date(), _sp()
	{
		g_opt = &this->m_refMain.options;

		{
			INITCOMMONCONTROLSEX _iccex;
			_iccex.dwSize = sizeof INITCOMMONCONTROLSEX;
			_iccex.dwICC = ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES;
			InitCommonControlsEx(&_iccex);
		}

		try
		{
			{
				int _w = 550;
				int _h = 435;
				int x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (_w / 2);
				int y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (_h / 2);

				this->m_window.create
				(
					0, WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
					this->m_refMain.m_window.getClassName(), "XimaSSS Options",
					x, y, _w, _h,
					this
				);
			}

			this->_general.create(this->m_window, MAKEINTRESOURCE(IDD_GENERAL), (DLGPROC)generalproc);
			this->_image.create(this->m_window, MAKEINTRESOURCE(IDD_IMAGE), (DLGPROC)imageproc);
			this->_ftp.create(this->m_window, MAKEINTRESOURCE(IDD_FTP), (DLGPROC)ftpproc);
			this->_date.create(this->m_window, MAKEINTRESOURCE(IDD_DATE), (DLGPROC)dateproc);
			this->_sp.create(this->m_window, MAKEINTRESOURCE(IDD_SPECIALPROGRAMS), (DLGPROC)spproc);

			this->_save.create(0, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, WC_BUTTON, "Save", 260, 375, 80, 25, 0, *this->m_window, (HMENU) 1);
		}
		catch(WindowException&)
		{
			delete this;
		}

		this->m_menu.insert(IDR_PNG1, *this, &GUI::onClickGeneral, 0, &GUI::onLeaveGeneral);
		this->m_menu.insert(IDR_PNG2, *this, &GUI::onClickImage, 0, &GUI::onLeaveImage);
		this->m_menu.insert(IDR_PNG3, *this, &GUI::onClickFTP, 0, &GUI::onLeaveFTP);
		this->m_menu.insert(IDR_PNG4, *this, &GUI::onClickDate, 0, &GUI::onLeaveDate);
		this->m_menu.insert(IDR_PNG5, *this, &GUI::onClickSP, 0, &GUI::onLeaveSP);

		this->m_menu.setDefault(0);
		this->m_menu.align(CENTER);
		this->m_menu.initiate(Color(255, 255, 255), Color(193, 210, 238), Color(224, 232, 246));

		this->m_window.show();
		this->m_window.toForeground();
	}

	GUI::~GUI(void)
	{
		this->_general.destroy();
		this->_image.destroy();
		this->_ftp.destroy();
		this->_date.destroy();
		this->_sp.destroy();

		this->_save.destroy();
		this->m_window.destroy();


		map<HWND, __tConStruct>::iterator iter;
		for(iter = tCons.begin(); iter != tCons.end(); iter++)
		{
			TerminateThread(iter->second.testThread, 0);
			TerminateThread(iter->second.testDotter, 0);
		}
		tCons.clear();
		FTP::FTPUploader::cleanup();

		this->m_refMain.m_refGUI = 0;
		g_opt = 0;
	}

	GUI& GUI::operator=(const GUI& _rhs)
	{
		if(this == &_rhs)
			return *this;

		this->m_refMain = _rhs.m_refMain;
		this->is_saving = _rhs.is_saving;

		this->m_menu = _rhs.m_menu;
		this->m_window = _rhs.m_window;
		this->_date = _rhs._date;
		this->_ftp = _rhs._ftp;
		this->_general = _rhs._general;
		this->_image = _rhs._image;
		this->_sp = _rhs._sp;
		return *this;
	}

	__inline long GUI::proc(Window &_window, const unsigned int &_message, const unsigned int &_wParam, const long &_lParam)
	{
		switch(_message)
		{
		case WM_KEYUP:
			if(_wParam == VK_TAB)
			{
				HDC _dc = GetDC(*_window);
				Graphics _gfx(_dc);
				this->m_menu.tab(_gfx);
				ReleaseDC(*_window, _dc);
			}
			break;
		case WM_COMMAND:
			if(HIWORD(_wParam) == BN_CLICKED && LOWORD(_wParam) == 1)
				this->save();
			break;
		case WM_LBUTTONUP:
		{
			HDC _dc = GetDC(*_window);
			Graphics _gfx(_dc);
			this->m_menu.routeClick(_gfx, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
			ReleaseDC(*_window, _dc);
			break;
		}
		case WM_MOUSEMOVE:
		{
			int xPos = GET_X_LPARAM(_lParam);
			int yPos = GET_Y_LPARAM(_lParam);

			if(!this->m_window.containsPoint(xPos, yPos))
				return 0;

			if(this->m_menu.isHoveredItemLeft(xPos, yPos))
			{
				HDC _dc = GetDC(*_window);
				Graphics _gfx(_dc);
				this->m_menu.routeLeave(_gfx);
				ReleaseDC(*_window, _dc);
				return 0;
			}
			if(this->m_menu.hasBounds(xPos, yPos))
			{
				HDC _dc = GetDC(*_window);
				Graphics _gfx(_dc);
				this->m_menu.routeHover(_gfx, xPos, yPos);
				ReleaseDC(*_window, _dc);
				return 0;
			}
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC _dc = BeginPaint(*_window, &ps);
			this->paint(_dc);
			EndPaint(*_window, &ps);
			break;
		}
		case WM_CLOSE:
			return this->close();
			break;
		case WM_DESTROY:
			break;
		default:
			return DefWindowProc(*_window, _message, _wParam, _lParam);
			break;
		}
		return 0;
	}

	__inline void GUI::paint(HDC& _dc)
	{
		// Make graphics object linked to the screen
		Graphics _gfx(_dc);

		{
			// Menu/Content Separator
			Pen _blackPen(Color(132, 132, 132), 1);
			Pen _whitePen(Color(200, 200, 200), 1);
			SolidBrush wBrush(Color(255, 255, 255));

			// Horizontal line
			_gfx.FillRectangle(&wBrush, 63, 366, 494, 41);
			_gfx.DrawLine(&_blackPen, 63, 363, 543, 363);
			_gfx.DrawLine(&_whitePen, 63, 364, 543, 364);
			_gfx.DrawLine(&_blackPen, 63, 365, 543, 365);
			_gfx.DrawLine(&_whitePen, 63, 366, 543, 366);

			// Vertical Line
			_gfx.FillRectangle(&wBrush, 55, 0, 5, 407);
			_gfx.DrawLine(&_blackPen, 60, 0, 60, 366);
			_gfx.DrawLine(&_whitePen, 61, 0, 61, 366);
			_gfx.DrawLine(&_blackPen, 62, 0, 62, 366);
			_gfx.DrawLine(&_whitePen, 63, 0, 63, 366);
		}

		// Paint the Menu
		this->m_menu.paint(_gfx);
	}

	__inline void GUI::onClickGeneral(void)
	{
		this->_general.show();
	}

	__inline void GUI::onLeaveGeneral(void)
	{
		this->_general.hide();
	}

	__inline void GUI::onClickImage(void)
	{
		this->_image.show();
	}

	__inline void GUI::onLeaveImage(void)
	{
		this->_image.hide();
	}

	__inline void GUI::onClickFTP(void)
	{
		this->_ftp.show();
	}

	__inline void GUI::onLeaveFTP(void)
	{
		this->_ftp.hide();
	}

	__inline void GUI::onClickDate(void)
	{
		this->_date.show();
	}

	__inline void GUI::onLeaveDate(void)
	{
		this->_date.hide();
	}

	__inline void GUI::onClickSP(void)
	{
		this->_sp.show();
	}

	__inline void GUI::onLeaveSP(void)
	{
		this->_sp.hide();
	}

	bool GUI::is_saving = false;
	__inline void GUI::save(void)
	{
		// is_saving critical section: avoiding duplication of method!
		if(is_saving)
			return;
		is_saving = true;
		{
			Options temp_options(this->m_refMain.options);
			{
				bool success = true;
				if(generalproc(*this->_general, GUI_SAVE, (unsigned int)&temp_options, 0) == 1)
				{
					Dialog::messageBox(this->m_window, "One or more options in the general section is incorrect", "Invalid value", MB_ICONQUESTION);
					success = false;
				}
				if(success && imageproc(*this->_image, GUI_SAVE, (unsigned int)&temp_options, -1) == 1)
				{
					Dialog::messageBox(this->m_window, "One or more options in the image section is incorrect", "Invalid value", MB_ICONQUESTION);
					success = false;
				}
				if(success && ftpproc(*this->_ftp, GUI_SAVE, (unsigned int)&temp_options, -1) == 1)
				{
					Dialog::messageBox(this->m_window, "One or more options in the ftp section is incorrect", "Invalid value", MB_ICONQUESTION);
					success = false;
				}
				if(success && dateproc(*this->_date, GUI_SAVE, (unsigned int)&temp_options, -1) == 1)
				{
					Dialog::messageBox(this->m_window, "One or more options in the date section is incorrect", "Invalid value", MB_ICONQUESTION);
					success = false;
				}
				if(success && spproc(*this->_sp, GUI_SAVE, (unsigned int)&temp_options, 0) == 1)
				{
					Dialog::messageBox(this->m_window, "One or more options in the special applications section is incorrect", "Invalid value", MB_ICONQUESTION);
					success = false;
				}
				if(!success) // failure, rewind save
				{
					is_saving = false;
					return;
				}
			}

			this->m_refMain.options = temp_options;
			const filesystem::path p = filesystem::initial_path();
			this->m_refMain.options.save(p.string() + "/options.xml");
		}

		if(IsDlgButtonChecked(*this->_general, IDC_AUTOSTART))
		{
			char buff[260];
			SHGetSpecialFolderPath(0, buff, 0x0007, 0);
			if(!filesystem::exists(filesystem::path(buff + string("\\XimaSSS.lnk"), filesystem::native)))
			{
				char PPath[100];
				GetModuleFileName(GetModuleHandle(0), PPath, 100);
				strcat_s(buff, sizeof buff, "\\XimaSSS.lnk");
				CreateShortCut(PPath, "", buff, "", SW_SHOWNORMAL, const_cast<char*>(filesystem::initial_path().string().c_str()), PPath, 0);
			}
		}
		else
		{
			char buff[260];
			SHGetSpecialFolderPath(0, buff, 0x0007, 0);
			filesystem::path p(buff + string("\\XimaSSS.lnk"), filesystem::native);
			if(filesystem::exists(p))
				filesystem::remove(p);
		}

		is_saving = false;
	}

	__inline bool GUI::close(void)
	{
		if(is_saving)
			return false;
		delete this;
			return true;
	}
}
#endif