#include "..\StdAfx.hpp"

#if defined(_WIN32)
   #include <windows.h>
   #include <winsock.h>

   #define socket_receive(Socket,buffer,len, flags) recv(Socket, (char*)buffer, len , flags)
   #define socket_send(Socket,buffer,len, flags) send(Socket, (char*)buffer, len , flags)
   #define socket_close closesocket
#else
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netdb.h>
   #include <arpa/inet.h>

   #define socket_receive(Socket,buffer,len, flags) recv(Socket, (char*)buffer, len , flags)
   #define socket_send(Socket,buffer,len, flags) send(Socket, (char*)buffer, len , flags)
   #define socket_close close
#endif

#pragma warning(disable: 4512)
#pragma warning(disable: 4127)
#pragma warning(disable: 4701)
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost;

#include "../Globals.func.hpp"
#include "../Win/Dialog.class.hpp"
#include "FTPUploader.class.hpp"

namespace FTP
{
	FTPUploader::FTPUploader(const string &_host, const string &_username, const string &_password, const int &_port)
		: m_host(_host), m_username(_username), m_password(_password), m_port(_port), hasLoggedIn(false)
	{
	}

	bool FTPUploader::initialize(void)
	{
		#if defined(_WIN32)
		WSADATA wsa;
		if(WSAStartup(MAKEWORD(1, 1), &wsa))
		{
			WSACleanup();
			return false;
		}
		#endif
		return true;
	}

	void FTPUploader::cleanup(void)
	{
		#if defined(_WIN32)
		WSACleanup();
		#endif
	}

	bool FTPUploader::login(void)
	{
		if(this->hasLoggedIn)
			return true;

		this->wSOCKET = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if( this->wSOCKET == INVALID_SOCKET )
			return false;

		hostent *he;
		if((he = gethostbyname( this->m_host.c_str() )) == 0)
			return false;

		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr( inet_ntoa( (**(in_addr**)he->h_addr_list ) ) );
		clientService.sin_port = htons((unsigned short)this->m_port);

		if( connect( this->wSOCKET, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR )
			return false;

		this->readReply();
		if(this->retValue != 220)
		{
			this->close();
			return false;
		}

		this->sendCommand("USER " + this->m_username);
		if( !(this->retValue == 331 || this->retValue == 230) )
			return false;

		if(this->retValue != 230)
		{
			this->sendCommand("PASS " + this->m_password);

			if( !(this->retValue == 230 || this->retValue == 202) )
			{
				this->close();
				return false;
			}
		}

		this->hasLoggedIn = true;
		return true;
	}

	void FTPUploader::close(void)
	{
		if( this->wSOCKET != 0 )
		{
			this->sendCommand("QUIT");
			socket_close(this->wSOCKET);
			this->wSOCKET = 0;
			this->hasLoggedIn = false;
		}
	}

	void FTPUploader::terminate(void)
	{
		if( this->wSOCKET != 0 )
		{
			socket_close(this->wSOCKET);
			this->wSOCKET = 0;
			this->hasLoggedIn = false;
		}
	}

	long FTPUploader::getFileSize(const string &fileName)
	{
		this->sendCommand("SIZE " + fileName);
		if(this->retValue == 213)
			return atol(this->reply.substr(4).c_str());
		else
			return 0;
	}

	bool FTPUploader::setMode(const bool &_mode)
	{
		if(_mode)
			this->sendCommand("TYPE I");
		else
			this->sendCommand("TYPE A");

		if(this->retValue != 200)
			return false;
		return true;
	}

	bool FTPUploader::changeDirectory(const string &_path)
	{
		if(_path == this->m_path)
			return true;

		this->sendCommand("CWD " + _path);
		if(this->retValue != 250)
			return false;

		this->m_path = _path;
		return true;
	}

	bool FTPUploader::makeDirectory(const string &_dirName)
	{
		this->sendCommand("MKD " + _dirName);
		if(this->retValue != 250)
			return false;
		return true;
	}

	bool FTPUploader::upload(const string &fileName, const bool &ftpContinue, Win::Dialog &_dlg, const string &totalString,
		const unsigned __int64 &bytesProcessed, const unsigned __int64 &totalSize, const int &exponent)
	{
		//static const int dataBlock = 5120; // 5 kB
		static const int dataBlock = 1024; // 1 kB

		if(!ftpContinue)
			throw 0;
		SOCKET cSocket = this->createDataSocket();
		if(!ftpContinue)
			throw 0;

		this->sendCommand("STOR " + fileName.substr(fileName.find_last_of("/\\") + 1));
		if(!ftpContinue)
			throw 0;
		if( !(this->retValue == 125 || this->retValue == 150) )
			return false;

		int length;
		char *filebuffer;
		{
			ifstream input;
			input.open( fileName.c_str() , ios::binary );
			if(!ftpContinue)
				throw 0;
			if(!input.is_open())
				return false;
			input.seekg (0, ios::end);
			if(!ftpContinue)
				throw 0;
			length = input.tellg();
			if(!ftpContinue)
				throw 0;
			input.seekg (0, ios::beg);
			if(!ftpContinue)
				throw 0;
			filebuffer = new char[length];
			if(!ftpContinue)
			{
				delete[] filebuffer;
				throw 0;
			}
			input.read(filebuffer, length);
			if(!ftpContinue)
			{
				delete[] filebuffer;
				throw 0;
			}
			input.close();
		}
		int nSent = 0;
		while(nSent < length)
		{
			if(!ftpContinue)
			{
				delete[] filebuffer;
				throw 0;
			}
			if( (length - nSent) < dataBlock)
				nSent += socket_send(cSocket, filebuffer + nSent, length - nSent, 0);
			else
				nSent += socket_send(cSocket, filebuffer + nSent, dataBlock, 0);
			if(!ftpContinue)
			{
				delete[] filebuffer;
				throw 0;
			}
			_dlg.setItemText(IDC_BYTESPROCESSED, dtos(round( (nSent + bytesProcessed) / pow(1024, (double)exponent), 2)) + totalString);
			if(!ftpContinue)
			{
				delete[] filebuffer;
				throw 0;
			}
			_dlg.getItem(IDC_BAR).sendMessage((WM_USER+2), int((double)(nSent + bytesProcessed) / totalSize * 10000), 0);
		}

		if(!ftpContinue)
		{
			delete[] filebuffer;
			throw 0;
		}
		delete[] filebuffer;
		if(!ftpContinue)
			throw 0;
		if(getpeername(cSocket,0,0))
			socket_close(cSocket);
		if(!ftpContinue)
			throw 0;
		this->readReply();
		if(!ftpContinue)
			throw 0;
		if( !(this->retValue == 226 || this->retValue == 250) )
			return false;
		return true;
	}

	bool FTPUploader::upload(const string &fileName, const bool &resume)
	{
		SOCKET cSocket = this->createDataSocket();
		long offset = 0;

		if(resume)
		{
			try
			{
				this->setMode(true);
				offset = this->getFileSize(fileName);
			}
			catch(bool)
			{
				offset = 0;
			}
		}

		if(offset > 0)
		{
			this->sendCommand("REST " + offset);
			if(this->retValue != 350)
				offset = 0;
		}

		this->sendCommand("STOR " + fileName.substr(fileName.find_last_of("/\\") + 1));
		if( !(this->retValue == 125 || this->retValue == 150) )
			return false;

		int length;
		char *filebuffer;
		{
			ifstream input;
			input.open( fileName.c_str() , ios::binary );
			input.seekg (0, ios::end);
			length = input.tellg();
			input.seekg (0, ios::beg);
			filebuffer = new char[length];
			input.read(filebuffer, length);
			input.close();
		}
		socket_send(cSocket, filebuffer, length, 0);
		delete[] filebuffer;
		if(getpeername(cSocket,0,0))
			socket_close(cSocket);

		this->readReply();
		if( !(this->retValue == 226 || this->retValue == 250) )
			return false;
		return true;
	}

	void FTPUploader::readReply(void)
	{
		this->message = "";
		this->reply = this->readLine();
		this->retValue = stoi(this->reply.substr(0,3));
	}

	string FTPUploader::readLine(void)
	{
		const static int BLOCK_SIZE = 512;
		char buffer[BLOCK_SIZE];
		int bytes;

		do
		{
			bytes = socket_receive(this->wSOCKET, buffer, BLOCK_SIZE ,0);
			this->message += buffer;
			this->message = this->message.substr(0, bytes);
		}
		while(!(bytes < sizeof buffer));

		vector<string> mess;
		split(mess,this->message,is_any_of("\n"));
		if(this->message.length() > 2)
			this->message = mess[mess.size() - 2];
		else
			this->message = mess[0];

		if(!(this->message.substr(3,1) == " "))
				return this->readLine();
		return this->message;
	}

	void FTPUploader::sendCommand(const string &c)
	{
		string command = c + "\r\n";
		const char *cmdBytes = command.c_str();
		socket_send(this->wSOCKET,cmdBytes, (int)strlen(cmdBytes),0);
		this->readReply();
	}

	SOCKET FTPUploader::createDataSocket(void)
	{
		this->sendCommand("PASV");
		if(this->retValue != 227)
			return 0;

		int index1 = (int)( this->reply.find_first_of('(', 0) );
		int index2 = (int)( this->reply.find_first_of(')', 0) );
		string ipData = this->reply.substr(index1 + 1,index2 - index1 - 1);
		int parts[6];
		int len = (int)ipData.length();
		int partCount = 0;
		string buf = "";
		for(int i = 0; i < len && partCount <= 6; i++)
		{
			string ch = ipData.substr(i,1);
			if( isdigit( lexical_cast<char>(ch) ) )
				buf += ch;
			else if(ch != ",")
				return 0;

			if(ch == "," || i + 1 == len)
			{
				try
				{
					parts[partCount++] = stoi(buf);
					buf = "";
				}
				catch(bad_lexical_cast &)
				{
					return 0;
				}
			}
		}

		string ipAddress = itos(parts[0]) + "." + itos( parts[1] ) + "." + itos( parts[2] ) + "." + itos( parts[3] );
		int port = (parts[4] << 8) + parts[5];
		SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if( s == INVALID_SOCKET )
			return 0;
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr( ipAddress.c_str() );
		clientService.sin_port = htons((unsigned short)port);
		if( connect( s, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR )
			return 0;
		return s;
	}
}