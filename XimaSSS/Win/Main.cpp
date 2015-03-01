#ifdef WIN32

#include "..\StdAfx.hpp"

#include "GUI.hpp"
#include "Main.hpp"

#include "../Globals.func.hpp"
#include <set> // FTPUpload...

namespace Win
{
	Main::Main(void) : options(), m_window(),
		m_capt(options, m_window), m_refGUI(0),
		m_stp(m_window, "XimaSSS", IDI_APPICON, NIF_MESSAGE | NIF_ICON | NIF_TIP,
			Win::NotifyIconData::ID, Win::NotifyIconData::NID_CBM),
		selectingWindow(false), selectingArea(false)
	{
	}

	Main::~Main()
	{
		if(this->m_refGUI != 0)
			delete this->m_refGUI;

		int _icon = GetClassLongPtr(this->m_window, GCLP_HICON);
		if(_icon)
			DestroyIcon((HICON)_icon);
		int _bg = GetClassLongPtr(this->m_window, GCLP_HBRBACKGROUND);
		if(_bg)
			DeleteObject((HBRUSH)_bg);

		this->m_window.unregisterHotKey(0x0000);
		this->m_window.unregisterHotKey(0x0001);
		this->m_window.unregisterHotKey(0x0002);
		this->m_window.unregisterHotKey(0x0003);
		this->m_window.unregisterHotKey(0x0004);
		this->m_window.unregisterHotKey(0x0005);
		this->m_window.destroy();
	}

	bool Main::initialize(WNDPROC _winProc, HINSTANCE__* _hInst, const string &_className)
	{
		{
			HICON hIcon = LoadIcon(_hInst, MAKEINTRESOURCE(IDI_APPICON));
			HBRUSH hBG = CreateSolidBrush(RGB(240, 240, 240));
			{
				Class _class(
					_winProc, _hInst,
					hIcon, hBG,
					_className,
					0, sizeof(Win::Control*)
				);
				if(!_class.reg())
					return false;
			}
			DeleteObject(hBG);

			try
			{
				this->m_window.create
				(
					0, 0,
					_className, "",
					0, 0, 0, 0,
					this, 0, 0, _hInst
				);
			}
			catch(WindowException&)
			{
				return false;
			}

			if(!this->m_stp.run())
				return false;

			DestroyIcon(hIcon);
		}

		{
			string path = filesystem::initial_path().string() + "/options.xml";
			if(!this->options.load(path))
			{
				this->options = Options(true);
				if(!this->options.save(path))
					return false;
			}
		}

		this->m_window.registerHotKey(0, VK_SNAPSHOT); // Virtual Multi Screen
		this->m_window.registerHotKey(MOD_ALT, VK_SNAPSHOT); // Primary Screen
		this->m_window.registerHotKey(MOD_SHIFT, VK_SNAPSHOT); // Window
		this->m_window.registerHotKey(MOD_SHIFT | MOD_CONTROL, VK_SNAPSHOT); // Foreground
		this->m_window.registerHotKey(MOD_CONTROL, VK_SNAPSHOT); // Area
		this->m_window.registerHotKey(MOD_ALT | MOD_CONTROL, VK_SNAPSHOT); // Working Area
		return true;
	}

	__inline long Main::proc(Window &_window, const unsigned int &_message, const unsigned int &_wParam, const long &_lParam)
	{
		static unsigned int TaskbarRestart;

		switch(_message)
		{
		case WM_CREATE:
			TaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
			break;
		case CW_WindowSelected:
			this->m_capt.window();
			this->selectingWindow = false;
			break;
		case CW_Abort:
			this->m_capt.abortWindow();
			this->selectingWindow = false;
			break;
		case Area_AreaSelected:
			this->m_capt.area();
			this->selectingArea = false;
			break;
		case Area_Abort:
			this->m_capt.abortArea();
			this->selectingArea = false;
			break;
		case WM_HOTKEY:
			switch(_wParam)
			{
			case 0x0000: // Virtual Multi Screen
				this->m_capt.multimonitor();
				break;
			case 0x0001: // Primary Screen
				this->m_capt.primary();
				break;
			case 0x0002: // Window
				if(!this->selectingWindow)
					this->m_capt.selectWindow(this->selectingWindow);
				break;
			case 0x0003: // Foreground
				this->m_capt.foreground();
				break;
			case 0x0004: // Area
				if(!this->selectingArea)
					this->m_capt.selectArea(this->selectingArea);
				break;
			case 0x0005: // Working Area
				this->m_capt.workingarea();
				break;
			}
			break;
		case NotifyIconData::NID_CBM:
			switch(this->m_stp.route(_lParam))
			{
			case OpenGUI:
			{
				if(this->m_refGUI)
					this->m_refGUI->m_window.toForeground(); // The GUI exists so make it foreground
				else
				{
					this->m_refGUI = new GUI(*this);
						return true;
				}
				break;
			}
			case Exit:
			{
				if(this->m_refGUI)
				{
					if(this->m_refGUI->close())
						PostQuitMessage(0);
				}
				else
					PostQuitMessage(0);
				break;
			}
			case FTPFiles: // FTP all files
				this->ftpAll();
				break;
			case MultiMonitor: // Virtual Multi Screen
				this->m_capt.multimonitor();
				break;
			case PrimaryScreen: // Primary Screen
				this->m_capt.primary();
				break;
			case CaptureWindow: // Window
				if(!this->selectingWindow)
					this->m_capt.selectWindow(this->selectingWindow);
				break;
			case Foreground: // Foreground
				this->m_capt.foreground();
				break;
			case Area: // Area
				if(!this->selectingArea)
					this->m_capt.selectArea(this->selectingArea);
				break;
			case WorkingArea: // Working Area
				this->m_capt.workingarea();
				break;
			default:
				return 0;
			}
			break;
		default:	
			if(_message == TaskbarRestart)
				return this->m_stp.run();
			return (long) DefWindowProc(*_window, _message, _wParam, _lParam);
			break;
		}
		return 0;
	}

	bool operator<(const Options::SpecialProgram &_lhs, const Options::SpecialProgram &_rhs)
	{
		if(_lhs.ftp.server < _rhs.ftp.server && _lhs.ftp.username < _rhs.ftp.username
			&& _lhs.ftp.password < _rhs.ftp.password && _lhs.ftp.port < _rhs.ftp.port)
			return true;
		else
			return false;
	}

	bool preComputeFiles(const string &_pName, const string &_path, int &_nFiles, unsigned long long &_totalSize, vector<string> &_locations, vector<unsigned long long> &_sizes)
	{
		string _p(_path);
		if(!(_path[_path.size() - 1] == '/' || _path[_path.size() - 1] == '\\'))
			_p += "/";

		ifstream _imageList((_p + _pName + ".ild").c_str());
		if(!_imageList.is_open())
			return false;

		const int n = 150;
		char temp[n];
		int count = 0;
		while(_imageList.getline(temp, 150))
		{
			if(temp[0] == ':' || temp[0] == 0)
				continue;
			try
			{
				filesystem::path _fPath(_p + temp, filesystem::native);
				if(!filesystem::exists(_fPath))
					continue;

				{
					unsigned long long _size = filesystem::file_size(_fPath);
					_totalSize += _size;
					_sizes.push_back(_size);
				}
				_locations.push_back(_fPath.string());
			}
			catch(filesystem::filesystem_error&)
			{
				continue;
			}

			count++;
		}
		_imageList.close();
		if(count > 0)
		{
			_nFiles += count;
			return true;
		}
		return false;
	}

	struct _server
	{
		struct _application
		{
			vector<string> _imageLocations;
			vector<unsigned long long> _imageSizes;
			string _path;
		};

		vector<_application> _apps;
		string _serverName;
		string _username;
		string _password;
		int _port;
	};

	bool ftpContinue = false;
	int __stdcall uploadproc(HWND hwnd, unsigned int _message, unsigned int _wParam, long)
	{
		switch(_message)
		{
		case WM_INITDIALOG:
			ftpContinue = true;
			return 1;
			break;
		case WM_COMMAND:
			if(_wParam == IDC_ABORT)
				ftpContinue = false;
		case WM_CLOSE:
			ftpContinue = false;
			break;
		case WM_APP + 1:
			DestroyWindow(hwnd);
			break;
		default:
			return 0;
		}
		return 0;
	}

	static HANDLE ftpthread = 0;
	static vector<_server> _servers;
	static unsigned long long _tSize;
	static int _nFiles;
	static int _exponent;
	static string totalSize = "";
	static Dialog _d;
	unsigned long __stdcall ftpRoutine(void*)
	{
		try
		{
			if(!ftpContinue)
				throw 0;

			int succeeded = 0;
			int failed = 0;
			int currentImage = 0;
			unsigned long long bytesprocessed = 0;

			vector<_server>::iterator _iter;
			for(_iter = _servers.begin(); _iter != _servers.end(); _iter++)
			{
				if(!ftpContinue)
					throw 0;
				_d.setItemText(IDC_FILE, string("Connecting to: ") + _iter->_serverName);
				if(!ftpContinue)
					throw 0;
				FTP::FTPUploader ftp(_iter->_serverName, _iter->_username, _iter->_password, _iter->_port);
				{
					if(!ftpContinue)
						throw 0;
					login:
					if(!ftp.login())
					{
						switch(Dialog::messageBox(_d, "Couldn't login to " + _iter->_serverName, "FTP Error", MB_TOPMOST | MB_SETFOREGROUND | MB_ICONEXCLAMATION | MB_CANCELTRYCONTINUE | MB_DEFBUTTON3))
						{
						case IDCANCEL:
							throw 0;
							break;
						case IDTRYAGAIN:
							goto login;
						case IDCONTINUE:
						{
							for(int _i = 0; _i < (int)_iter->_apps.size(); _i++)
							{
								if(!ftpContinue)
									throw 0;
								int inc = _iter->_apps[_i]._imageLocations.size();
								failed += inc;
								currentImage += inc;
								for(int _i2 = 0; _i2 < (int)_iter->_apps[_i]._imageSizes.size(); _i2++)
								{
									if(!ftpContinue)
										throw 0;
									bytesprocessed += _iter->_apps[_i]._imageSizes[_i2];
								}
								if(!ftpContinue)
									throw 0;
							}
							_servers.erase(_iter);
							_d.setItemText(IDC_NFAIL, "Failed Files: " + itos(failed));
							if(!ftpContinue)
								throw 0;
							_d.setItemText(IDC_XOFY, "Image " + itos(currentImage) + " of " + itos(_nFiles));
							if(!ftpContinue)
								throw 0;
							_d.setItemText(IDC_BYTESPROCESSED, dtos(round(bytesprocessed / pow(1024, (double)_exponent), 2)) + totalSize);
							if(!ftpContinue)
								throw 0;
							_d.getItem(IDC_BAR).sendMessage((WM_USER+2), int((double)bytesprocessed / _tSize * 10000), 0);
							if(!ftpContinue)
								throw 0;
							continue;
						}
						default:
							throw 0;
							break;
						}
					}
					else
					{
						if(!ftpContinue)
							throw 0;
						_d.setItemText(IDC_FILE, string("Successfully connected to: ") + _iter->_serverName);
					}
				}

				vector<_server::_application>::iterator _appIter;
				for(_appIter = _iter->_apps.begin(); _appIter != _iter->_apps.end(); _appIter++)
				{
					if(!ftpContinue)
						throw 0;
					if(_appIter->_path[0] != '/')
						_appIter->_path = "/" + _appIter->_path;
					{
						CWD:
						if(!ftp.changeDirectory('/' + _appIter->_path))
						{
							switch(Dialog::messageBox(_d, "Couldn't change the working directory to: " + _appIter->_path, "FTP Error", MB_TOPMOST | MB_SETFOREGROUND | MB_ICONEXCLAMATION | MB_CANCELTRYCONTINUE | MB_DEFBUTTON3))
							{
							case IDCANCEL:
								throw 0;
								break;
							case IDTRYAGAIN:
								goto CWD;
							case IDCONTINUE:
							{
								if(!ftpContinue)
									throw 0;
								int inc = _appIter->_imageLocations.size();
								failed += inc;
								currentImage += inc;
								for(int _i = 0; _i < (int)_appIter->_imageSizes.size(); _i++)
								{
									if(!ftpContinue)
										throw 0;
									bytesprocessed += _appIter->_imageSizes[_i];
								}
								if(!ftpContinue)
									throw 0;
								_d.setItemText(IDC_NFAIL, "Failed Files: " + itos(failed));
								if(!ftpContinue)
									throw 0;
								_d.setItemText(IDC_XOFY, "Image " + itos(currentImage) + " of " + itos(_nFiles));
								if(!ftpContinue)
									throw 0;
								_d.setItemText(IDC_BYTESPROCESSED, dtos(round(bytesprocessed / pow(1024, (double)_exponent), 2)) + totalSize);
								if(!ftpContinue)
									throw 0;
								_d.getItem(IDC_BAR).sendMessage((WM_USER+2), int((double)bytesprocessed / _tSize * 10000), 0);
								_iter->_apps.erase(_appIter);
								continue;
							}
							default:
								throw 0;
								break;
							}
						}
					}

					if(!ftpContinue)
						throw 0;
					for(int _imageIndex = 0; _imageIndex < (int)_appIter->_imageLocations.size(); _imageIndex++)
					{
						if(!ftpContinue)
							throw 0;
						string _file = _appIter->_imageLocations[_imageIndex];
						currentImage++;
						_d.setItemText(IDC_XOFY, "Image " + itos(currentImage) + " of " + itos(_nFiles));
						if(!ftpContinue)
							throw 0;
						_d.setItemText(IDC_FILE, string("Uploading: ") + _file);
						try
						{
							if(ftp.upload(_file, ftpContinue, _d, totalSize, bytesprocessed, _tSize, _exponent))
							{
								if(!ftpContinue)
									throw 0;
								succeeded++;
								_d.setItemText(IDC_FILE, "Success...");
								if(!ftpContinue)
									throw 0;
								_d.setItemText(IDC_NSUCC, "Succeeded Files: " + itos(succeeded));
							}
							else
							{
								if(!ftpContinue)
									throw 0;
								failed++;
								_d.setItemText(IDC_FILE, "Failiure...");
								if(!ftpContinue)
									throw 0;
								_d.setItemText(IDC_NFAIL, "Failed Files: " + itos(failed));
							}
							if(!ftpContinue)
								throw 0;
						}
						catch(...)
						{
							throw;
						}

						bytesprocessed += _appIter->_imageSizes[_imageIndex];
						_d.setItemText(IDC_BYTESPROCESSED, dtos(round(bytesprocessed / pow(1024, (double)_exponent), 2)) + totalSize);
						if(!ftpContinue)
							throw 0;
						_d.getItem(IDC_BAR).sendMessage((WM_USER+2), int((double)bytesprocessed / _tSize * 10000), 0);
					}
					if(!ftpContinue)
						throw 0;
					_iter->_apps.erase(_appIter);
				}
				_d.setItemText(IDC_FILE, string("Dissconnecting from: ") + _iter->_serverName);
				_servers.erase(_iter);
				if(!ftpContinue)
					throw 0;
				ftp.close();
			}
			FTP::FTPUploader::cleanup();
			_servers.clear();
			totalSize.clear();
			_tSize = 0;
			_nFiles = 0;
			_exponent = 0;
			_d.sendMessage(WM_APP + 1, 0, 0);
			return 0;
		}
		catch(...)
		{
			FTP::FTPUploader::cleanup();
			_servers.clear();
			totalSize.clear();
			_tSize = 0;
			_nFiles = 0;
			_exponent = 0;
			_d.sendMessage(WM_APP + 1, 0, 0);
			return 0;
		}
	}

	void Main::ftpAll(void)
	{
		{
			set<string> _cache;
			if(this->options.ftp.enable)
			{
				_server _general;
				{
					_server::_application _app;
					if(preComputeFiles("~GENERAL~", this->options.path, _nFiles, _tSize, _app._imageLocations, _app._imageSizes))
					{
						_app._path = this->options.ftp.path;
						_general._apps.push_back(_app);
					}
				}
				{
					map<string, Options::SpecialProgram>::iterator iter;
					for(iter = this->options.specialPrograms.begin(); iter != this->options.specialPrograms.end(); iter++)
						if(iter->second.useDefaultFTP)
						{
							_server::_application _app;
							if(preComputeFiles(iter->first, iter->second.path, _nFiles, _tSize, _app._imageLocations, _app._imageSizes))
							{
								_app._path = this->options.ftp.path;
								_general._apps.push_back(_app);
							}
						}
				}
				{
					map<string, Options::SpecialProgram>::iterator iter;
					for(iter = this->options.specialPrograms.begin(); iter != this->options.specialPrograms.end(); iter++)
					{
						if(iter->second.ftp.enable && !iter->second.useDefaultFTP
							&& iter->second.ftp.server == this->options.ftp.server
							&& iter->second.ftp.username == this->options.ftp.username
							&& iter->second.ftp.password == this->options.ftp.password
							&& iter->second.ftp.port == this->options.ftp.port)
						{
							_server::_application _app;
							if(preComputeFiles(iter->first, iter->second.path, _nFiles, _tSize, _app._imageLocations, _app._imageSizes))
							{
								_app._path = iter->second.ftp.path;
								_general._apps.push_back(_app);
								_cache.insert(iter->first);
							}
						}
					}
				}

				if(!_general._apps.empty())
				{
					_general._serverName = this->options.ftp.server;
					_general._username = this->options.ftp.username;
					_general._password = this->options.ftp.password;
					_general._port = this->options.ftp.port;
					_servers.push_back(_general);
				}
			}
			{
				set<Options::SpecialProgram> _cache2;
				{
					map<string, Options::SpecialProgram>::iterator iter;
					for(iter = this->options.specialPrograms.begin(); iter != this->options.specialPrograms.end(); iter++)
					{
						if((_cache.find(iter->first) != _cache.end()) || iter->second.useDefaultFTP || !iter->second.ftp.enable)
							continue;

						Options::SpecialProgram _sp;
						_sp.ftp.password = iter->second.ftp.password;
						_sp.ftp.username = iter->second.ftp.username;
						_sp.ftp.server = iter->second.ftp.server;
						_sp.ftp.port = iter->second.ftp.port;

						if(_cache2.find(_sp) != _cache2.end())
							continue;
						_cache2.insert(_sp);
					}
				}
				set<Options::SpecialProgram>::iterator iterOuter;
				for(iterOuter = _cache2.begin(); iterOuter != _cache2.end(); iterOuter++)
				{
					_server _s;
					map<string, Options::SpecialProgram>::iterator iter;
					for(iter = this->options.specialPrograms.begin(); iter != this->options.specialPrograms.end(); iter++)
					{
						if(iter->second.ftp.password != iterOuter->ftp.password
							|| iter->second.ftp.username != iterOuter->ftp.username
							|| iter->second.ftp.server != iterOuter->ftp.server
							|| iter->second.ftp.port != iterOuter->ftp.port)
							continue;

						_server::_application _app;
						if(preComputeFiles(iter->first, iter->second.path, _nFiles, _tSize, _app._imageLocations, _app._imageSizes))
						{
							_app._path = iter->second.ftp.path;
							_s._apps.push_back(_app);
						}
					}
					if(!_s._apps.empty())
					{
						_s._password = iterOuter->ftp.password;
						_s._username = iterOuter->ftp.username;
						_s._serverName = iterOuter->ftp.server;
						_s._port = iterOuter->ftp.port;
						_servers.push_back(_s);
					}
				}
			}
		}
		if(_servers.empty())
			return;

		{
			string sizeUnit = " Bytes";
			double _totalSize = (double)_tSize;
			for(; _totalSize > 1024; _totalSize /= 1024, ++_exponent);
			{
				switch(_exponent)
				{
				case 1:
					sizeUnit = " kB";
					break;
				case 2:
					sizeUnit = " MB";
					break;
				case 3:
					sizeUnit = " GB";
					break;
				default:
					sizeUnit = " Unknown Unit";
					break;
				}
			}
			totalSize = " of " + dtos(round(_totalSize, 2)) + sizeUnit;
		}

		if(!FTP::FTPUploader::initialize())
		{
			Dialog::messageBox(this->m_window, "Couldn't initialize Sockets library", "FTP Error", MB_TOPMOST | MB_SETFOREGROUND | MB_ICONEXCLAMATION | MB_OK);
			_servers.clear();
			totalSize.clear();
			return;
		}

		_d.create(this->m_window, MAKEINTRESOURCE(IDD_FTPTRANSFER), uploadproc);
		_d.getItem(IDC_BAR).sendMessage((WM_USER+1), 0, MAKELPARAM(0, 10000));
		_d.setItemText(IDC_XOFY, "Image " + itos(0) + " of " + itos(_nFiles));
		_d.setItemText(IDC_BYTESPROCESSED, "0" + totalSize);
		ftpthread = CreateThread(0, 0, ftpRoutine, 0, 0, 0);
	}
}
#endif