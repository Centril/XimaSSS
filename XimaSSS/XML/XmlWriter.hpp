//////////////////////////////////////////////////////////////////////
//
// xmlparser.h: interface & implementation for the XmlStream class.
// 
// Author: Oboltus, December 2003
//
// This code is provided "as is", with absolutely no warranty expressed
// or implied. Any use is at your own risk.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XMLWRITER_HPP
#define __XMLWRITER_HPP

// disable terrible MSVC warnings which are issued when using STL
#ifdef	_MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4514 )
#pragma warning( disable : 4511 )
#pragma warning( disable : 4512 )
#endif

#include	<stack>
#include	<string>
#include	<sstream>
#include	<fstream>

namespace XmlWriter
{
	class XmlStream
	{
	public:
		// XML version constants
		enum
		{
			versionMajor = 1, versionMinor = 0
		};

		// Internal helper class
		struct Controller
		{
			typedef	enum
			{
				whatProlog, whatComment, whatTag, whatTagEnd, whatTextTagEnd, whatAttribute, whatCharData, whatText
			}	what_type;

			what_type	what;
			std::string str;

			inline Controller(const Controller& c) : what(c.what), str(c.str)
			{
			}

			inline Controller(const what_type _what) : what(_what)
			{
			}
			
			// use template constructor because string field <str> may be initialized 
			// from different sources: char*, std::string etc
			template<class t> inline Controller(const what_type _what, const t& _str) : what(_what), str(_str)
			{
			}
		};

		// XmlStream refers std::ostream object to perform actual output operations
		inline XmlStream(std::ostream&	_s) : s(_s), state(stateNone), prologWritten(false)
		{
		}

	public:

		// Before destroying check whether all the open tags are closed
		~XmlStream()
		{
			if(stateTagName == state)
			{
				s << "/>\n";
				state = stateNone;
			}
			while(tags.size())
				endTag(tags.top());
		}

		// default behaviour - delegate object output to std::stream
		template<class t>
		XmlStream& operator<<(const t& value)
		{
			if(stateTagName == state)
				tagName << value;
			s << value;
			return *this;
		}

		// this is the main working horse
		// and it's long a little
		XmlStream& operator<<(const Controller& controller) {

			switch(controller.what)
			{
			case Controller::whatProlog:
				if(!prologWritten && stateNone == state)
				{
					s << "<?xml version=\"" << versionMajor << '.' << versionMinor << "\"?>";
					prologWritten = true;
				}
				break;	//	Controller::whatProlog
			case Controller::whatComment:
				s << "\n<!-- " << controller.str << " -->";
				break;
			case Controller::whatTag:
				closeTagStart();
				s << "\n";
				if(tags.size() > 0)
					for(unsigned int i = 0; i < tags.size(); i++)
						s << '\t';
				s << "<";
				if(controller.str.empty())
				{
					clearTagName();
					state = stateTagName;
				}
				else
				{
					s << controller.str;
					tags.push(controller.str);
					state = stateTag;
				}
				break;	//	Controller::whatTag

			case Controller::whatTagEnd:
				endTag(controller.str);
				break;	//	Controller::whatTagEnd

			case Controller::whatTextTagEnd:
				endTag(controller.str, false);
				break;	//	Controller::whatTagEnd

			case Controller::whatAttribute:
				switch (state)
				{
				case stateTagName:
					tags.push(tagName.str());
					break;

				case stateAttribute:
					s << '\"';
				}

				if (stateNone != state)
				{
					s << ' ' << controller.str << "=\"";
					state = stateAttribute;
				}
				// else throw some error - unexpected attribute (out of any tag)

				break;	//	Controller::whatAttribute

			case Controller::whatCharData:
				closeTagStart();
				s << '\n';
				for(unsigned int i = 0; i < tags.size(); i++)
					s << '\t';

				state = stateNone;
				break;	//	Controller::whatCharData
			case Controller::whatText:
				closeTagStart();
				state = stateNone;
				break;
			}

			return	*this;
		}

	private:
		// state of the stream 
		typedef	enum
		{
			stateNone, stateTag, stateAttribute, stateTagName
		}	state_type;

		// tag name stack
		typedef std::stack<std::string>	tag_stack_type;

		tag_stack_type tags;
		state_type state;
		std::ostream& s;
		bool prologWritten;
		std::ostringstream tagName;

		// I don't know any way easier (legal) to clear std::stringstream...
		inline void clearTagName()
		{
			const std::string	empty_str;
			tagName.rdbuf()->str(empty_str);
		}

		// Close current tag
		void closeTagStart(bool self_closed = false)
		{
			if(stateTagName == state)
				tags.push(tagName.str());

			// note: absence of 'break's is not an error
			switch(state)
			{
			case stateAttribute:
				s << '\"';

			case stateTagName:
			case stateTag:
				if (self_closed)
					s << " />";
				else
					s << ">";
			}
		}

		// Close tag (may be with closing all of its children)
		void endTag(const std::string& tag, bool normaltag = true)
		{
			bool brk = false;

			while(tags.size() > 0 && !brk)
			{
				if(stateNone == state)
				{
					if(normaltag)
					{
						s << "\n";
						for(unsigned int i = 1; i < tags.size(); i++)
							s << '\t';
					}
					s << "</" << tags.top() << ">";
				}
				else {
					closeTagStart(true);
					state = stateNone;
				}
				brk = tag.empty() || tag == tags.top();
				tags.pop();

			}
		}
	};	//	class XmlStream

	// Helper functions, they may be simply overwritten
	// E.g. you may use std::string instead of const char*

	inline const XmlStream::Controller prolog()
	{
		return XmlStream::Controller(XmlStream::Controller::whatProlog);
	}

	inline const XmlStream::Controller comment(const char* const comment)
	{
		return XmlStream::Controller(XmlStream::Controller::whatComment, comment);
	}

	inline const XmlStream::Controller tag()
	{
		return XmlStream::Controller(XmlStream::Controller::whatTag);
	}

	inline const XmlStream::Controller tag(const char* const tag_name)
	{
		return XmlStream::Controller(XmlStream::Controller::whatTag, tag_name);
	}

	inline const XmlStream::Controller endtag()
	{
		return XmlStream::Controller(XmlStream::Controller::whatTagEnd);
	}

	inline const XmlStream::Controller endtexttag()
	{
		return XmlStream::Controller(XmlStream::Controller::whatTextTagEnd);
	}

	inline const XmlStream::Controller endtag(const char* const tag_name)
	{
		return XmlStream::Controller(XmlStream::Controller::whatTagEnd, tag_name);
	}

	inline const XmlStream::Controller attr(const char* const attr_name)
	{
		return XmlStream::Controller(XmlStream::Controller::whatAttribute, attr_name);
	}

	inline const XmlStream::Controller chardata()
	{
		return XmlStream::Controller(XmlStream::Controller::whatCharData);
	}

	inline const XmlStream::Controller text()
	{
		return XmlStream::Controller(XmlStream::Controller::whatText);
	}
}	// namespace

#endif	//	__XMLWRITER_HPP