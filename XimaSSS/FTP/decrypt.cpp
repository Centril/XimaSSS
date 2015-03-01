#include "..\StdAfx.hpp"
#include "base64.hpp"
#include "encrypt.hpp"

namespace FTP
{
	string encrypt(const string &str)
	{
		string key;
		int lowest = 32, highest = 126;
		int range = ( highest - lowest ) + 1;
		for(int index = 0; index < 10; index++)
			key += static_cast<char>(lowest + int( range * rand() / (RAND_MAX + 1.0) ));

		string encoded = base64_encode(reinterpret_cast<const unsigned char*>(key.c_str()), key.length());
		encoded = encoded.substr(0, encoded.length() - 2);

		encoded += "A~B~C" + base64_encode(reinterpret_cast<const unsigned char*>(str.c_str()), str.length());
		encoded = base64_encode(reinterpret_cast<const unsigned char*>(encoded.c_str()), encoded.length());
		return encoded.substr(0, encoded.length() - 2);
	}

	string decrypt(const string &str)
	{
		if(str == "")
			return "";
		string decoded = base64_decode(str);
		return base64_decode(decoded.substr(decoded.find("A~B~C") + 5, decoded.length()));
	}
}