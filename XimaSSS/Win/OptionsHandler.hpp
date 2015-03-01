#ifndef _OPTIONSHANDLER_HPP // Inclution guard
#define _OPTIONSHANDLER_HPP

#ifdef WIN32
namespace Win
{
	struct Options
	{
		string path, filenameFormat;
		struct Image
		{
			string fileType, unit;
			float width, height;
			int resolution, quality;
			bool resize;

			Image(const bool &dummy = false);
		} image;

		struct Date
		{
			bool enable;
			int xPos, yPos;
			string textformat, fontName;
			int fontSize, fontStyle, opacity;
			int fontColor[3];

			Date(const bool &dummy = false);
		} date;

		struct FTP
		{
			bool enable, autosend;
			string server, path;
			int port;
			string username,password;

			FTP(const bool &dummy = false);
		} ftp;

		struct SpecialProgram
		{
			string name, executablePath, path, filenameFormat;

			bool useDefaultImage;
			Image image;
			bool useDefaultDate;
			Date date;
			bool useDefaultFTP;
			FTP ftp;
		};

		map<string, SpecialProgram> specialPrograms;

		Options(void);
		Options(const bool &dummy);
		~Options(void);

		bool save(const string &filename);
		bool load(const string &filename);
		Options(const Options& o);
		Options& Options::operator=(const Options& _rhs);
	};
}
#endif
#endif