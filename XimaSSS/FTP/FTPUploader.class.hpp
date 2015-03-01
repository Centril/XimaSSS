#ifndef _FTPUPLOADER_HPP // Inclution guard
#define _FTPUPLOADER_HPP

namespace FTP
{
	class FTPUploader
	{
	public:
		string m_host, m_username, m_password, m_path;
		int m_port;
		bool hasLoggedIn;

		SOCKET wSOCKET;
		int retValue;
		string message, reply;

	public:
		FTPUploader(const string &_host, const string &_username, const string &_password, const int &_port);
		static bool initialize(void);
		bool login(void);
		void close(void);
		void terminate(void);
		static void cleanup(void);

		long getFileSize(const string &fileName);
		bool setMode(const bool &_mode);
		bool changeDirectory(const string &_path);
		bool makeDirectory(const string &_dirName);
		bool upload(const string &fileName, const bool &resume = false);
		bool upload(const string &fileName, const bool &ftpContinue, Win::Dialog &_dlg, const string &totalString,
			const unsigned __int64 &bytesProcessed, const unsigned __int64 &totalSize, const int &exponent);

	private:
		void readReply(void);
		string readLine(void);
		void sendCommand(const string &c);
		SOCKET createDataSocket(void);
	};
}

#endif