#ifndef _BASE64_HPP // Inclution guard
#define _BASE64_HPP

namespace FTP
{
	std::string base64_encode(unsigned char const* , unsigned int len);
	std::string base64_decode(std::string const &s);
}
#endif