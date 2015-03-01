#ifdef WIN32

#include "..\StdAfx.hpp"

#include "../Globals.func.hpp"
#include "../FTP/encrypt.hpp"

#define TIXML_USE_TICPP
#define TIXML_USE_STL
#include "..\XML/tinyxml.hpp"
#include "..\XML/XmlWriter.hpp"

#include "OptionsHandler.hpp"

namespace Win
{
	Options::Options(void)
	{
	}

	Options::Options(const bool &dummy)
	{
		UNREFERENCED_PARAMETER(dummy);
		this->path = filesystem::initial_path().string() + "/Images";
		this->filenameFormat = "%date%-%interval(1,100)%";
		this->image = Image(true);
		this->date = Date(true);
		this->ftp = FTP(true);
	}

	Options::Image::Image(const bool &dummy)
	{
		if(dummy)
		{
			this->fileType = "Joint Photographic Experts Group | JPEG";
			this->unit = "pixels";
			this->quality = 100;
			this->resize = false;
			this->resolution = 72;
			this->width = (float)GetSystemMetrics(SM_CXSCREEN);
			this->height = (float)GetSystemMetrics(SM_CYSCREEN);
		}
	}

	Options::Date::Date(const bool &dummy)
	{
		if(dummy)
		{
			this->enable = true;
			this->opacity = 100;
			this->textformat = "%dd%/%MM%-%yyyy% %hh%:%mm%";
			this->xPos = 50;
			this->yPos = 50;
			this->fontName = "Verdana";
			this->fontColor[0] = 0;
			this->fontColor[1] = 0;
			this->fontColor[2] = 0;
			this->fontSize = 12;
			this->fontStyle = makeStyle(1, 700);
		}
	}

	Options::FTP::FTP(const bool &dummy)
	{
		if(dummy)
		{
			this->enable = false;
			this->autosend = false;
			this->server = "ftp.yourdomain.com";
			this->path = "/Screenshots/";
			this->username = "";
			this->password = "";
			this->port = 21;
		}
	}

	Options::~Options(void)
	{
	}

	Options::Options(const Options& o)
	{
		this->date = o.date;
		this->filenameFormat = o.filenameFormat;
		this->ftp = o.ftp;
		this->image = o.image;
		this->path = o.path;
		this->specialPrograms = o.specialPrograms;
	}

	Options& Options::operator=(const Options& _rhs)
	{
		if(this == &_rhs)
			return *this;

		this->date = _rhs.date;
		this->filenameFormat = _rhs.filenameFormat;
		this->ftp = _rhs.ftp;
		this->image = _rhs.image;
		this->path = _rhs.path;
		this->specialPrograms = _rhs.specialPrograms;
		return *this;
	}

	bool Options::save(const string &filename)
	{
		using namespace XmlWriter;
		ofstream f(filename.c_str());
		if(!f.good())
			return false;

		XmlStream xml(f);
		xml
			<< prolog()
			<< comment("Warning: Do not edit in this file unless you know what you're doing\n\t Editing this file might cause the program to corrupt and crash")
			<< tag("options")
				<< tag("path") << text() << this->path << endtexttag()
				<< tag("format") << text() << this->filenameFormat << endtexttag()
				<< tag("image")
					<< attr("unit") << this->image.unit
					<< attr("resize") << this->image.resize
					<< attr("quality") << this->image.quality
					<< attr("resolution") << this->image.resolution
					<< attr("filetype") << this->image.fileType
					<< tag("width") << text() << this->image.width << endtexttag()
					<< tag("height") << text() << this->image.height << endtexttag()
				<< endtag()
				<< tag("date")
					<< attr("enable") << this->date.enable
					<< attr("xpos") << this->date.xPos
					<< attr("ypos") << this->date.yPos
					<< tag("textformat") << text() << this->date.textformat << endtexttag()
					<< tag("opacity") << text() << this->date.opacity << endtexttag()
					<< tag("font")
						<< attr("size") << this->date.fontSize
						<< attr("style") << this->date.fontStyle
						<< attr("color") << itos(this->date.fontColor[0]) + '|' + itos(this->date.fontColor[1]) + '|' + itos(this->date.fontColor[2])
						<< text() << this->date.fontName
					<< endtexttag()
				<< endtag()
				<< tag("ftp")
					<< attr("enable") << this->ftp.enable
					<< attr("autosend") << this->ftp.autosend
					<< tag("server")
						<< attr("username") << this->ftp.username
						<< attr("password") << ::FTP::encrypt(this->ftp.password)
						<< attr("port") << this->ftp.port
						<< text() << this->ftp.server
					<< endtexttag()
					<< tag("path") << text() << this->ftp.path << endtexttag()
				<< endtag()
				<< tag("specialprograms");
				map<string, Options::SpecialProgram>::iterator iter;
				for(iter = this->specialPrograms.begin(); iter != this->specialPrograms.end(); iter++)
				{
					SpecialProgram &program = iter->second;
					xml
						<< tag("specialprogram")
							<< attr("name") << program.name
							<< tag("programpath") << text() << program.executablePath << endtexttag()
							<< tag("path") << text() << program.path << endtexttag()
							<< tag("format") << text() << program.filenameFormat << endtexttag()
							<< tag("usedefaultimage") << text() << program.useDefaultImage << endtexttag()
							<< tag("usedefaultdate") << text() << program.useDefaultDate << endtexttag()
							<< tag("usedefaultftp") << text() << program.useDefaultFTP << endtexttag();
					if(!program.useDefaultImage)
					{
						xml
							<< tag("image")
								<< attr("unit") << program.image.unit
								<< attr("resize") << program.image.resize
								<< attr("quality") << program.image.quality
								<< attr("resolution") << program.image.resolution
								<< attr("filetype") << program.image.fileType
								<< tag("width") << text() << program.image.width << endtexttag()
								<< tag("height") << text() << program.image.height << endtexttag()
							<< endtag();
					}
					if(!program.useDefaultDate)
					{
						xml
							<< tag("date")
								<< attr("enable") << program.date.enable
								<< attr("xpos") << program.date.xPos
								<< attr("ypos") << program.date.yPos
								<< tag("textformat") << text() << program.date.textformat << endtexttag()
								<< tag("opacity") << text() << program.date.opacity << endtexttag()
								<< tag("font")
									<< attr("size") << program.date.fontSize
									<< attr("style") << program.date.fontStyle
									<< attr("color") << itos(program.date.fontColor[0]) + '|' + itos(program.date.fontColor[1]) + '|' + itos(program.date.fontColor[2])
									<< text() << program.date.fontName
								<< endtexttag()
							<< endtag();
					}
					if(!program.useDefaultFTP)
					{
						xml
							<< tag("ftp")
								<< attr("enable") << program.ftp.enable
								<< attr("autosend") << program.ftp.autosend
								<< tag("server")
									<< attr("username") << program.ftp.username
									<< attr("password") << ::FTP::encrypt(program.ftp.password)
									<< attr("port") << program.ftp.port
									<< text() << program.ftp.server
								<< endtexttag()
								<< tag("path") << text() << program.ftp.path << endtexttag()
							<< endtag();
					}
					xml
						<< endtag();
				}
		return true;
	}

	bool Options::load(const string &filename)
	{
		TiXmlDocument doc(filename);
		if(!doc.LoadFile())
			return false;

		TiXmlElement* root = doc.FirstChildElement("options");

	try
	{
		this->path = root->FirstChildElement("path")->GetText();
		this->filenameFormat = root->FirstChildElement("format")->GetText();

		TiXmlElement* img = root->FirstChildElement("image");
		this->image.unit = img->Attribute("unit");
		this->image.fileType = img->Attribute("filetype");
		this->image.resize = stob(img->Attribute("resize"));
		this->image.quality = stoi(img->Attribute("quality"));
		this->image.resolution = stoi(img->Attribute("resolution"));

		TiXmlElement* width = img->FirstChildElement("width");
		this->image.width = stof(width->GetText());
		TiXmlElement* height = img->FirstChildElement("height");
		this->image.height = stof(height->GetText());

		TiXmlElement* d = root->FirstChildElement("date");
		this->date.enable = stob(d->Attribute("enable"));
		this->date.xPos = stoi(d->Attribute("xpos"));
		this->date.yPos = stoi(d->Attribute("ypos"));

		this->date.textformat = d->FirstChildElement("textformat")->GetText();
		this->date.opacity = stoi(d->FirstChildElement("opacity")->GetText());

		TiXmlElement* font = d->FirstChildElement("font");
		this->date.fontName = font->GetText();
		this->date.fontSize = stoi(font->Attribute("size"));
		this->date.fontStyle = stoi(font->Attribute("style"));
		string color = font->Attribute("color");
		this->date.fontColor[0] = stoi(color.substr(0, color.find_first_of("|")).substr(0, 3));
		this->date.fontColor[0] = this->date.fontColor[0] > 255 ? 255 : this->date.fontColor[0];
		this->date.fontColor[1] = stoi(color.substr(color.find_first_of("|") + 1, color.find_last_of("|") - 4).substr(0, 3));
		this->date.fontColor[1] = this->date.fontColor[1] > 255 ? 255 : this->date.fontColor[1];
		this->date.fontColor[2] = stoi(color.substr(color.find_last_of("|") + 1, 3));
		this->date.fontColor[2] = this->date.fontColor[2] > 255 ? 255 : this->date.fontColor[2];

		TiXmlElement* f = root->FirstChildElement("ftp");
		this->ftp.enable = stob(f->Attribute("enable"));
		this->ftp.autosend = stob(f->Attribute("autosend"));
		TiXmlElement* server = f->FirstChildElement("server");
		this->ftp.server = server->GetText() != 0 ? server->GetText() : "";
		this->ftp.username = server->Attribute("username");
		this->ftp.password = ::FTP::decrypt(server->Attribute("password"));
		this->ftp.port = stoi(server->Attribute("port"));
		this->ftp.path = f->FirstChildElement("path")->GetText() != 0 ? f->FirstChildElement("path")->GetText() : "";

		TiXmlElement* sps = root->FirstChildElement("specialprograms");
		for(TiXmlElement* child = sps->FirstChildElement("specialprogram"); child; child = child->NextSiblingElement())
		{
			SpecialProgram sp;
			sp.name = child->Attribute("name");
			if(sp.name == "~GENERAL~") // ~GENERAL~ IS RESERVED!!!
				continue;
			sp.executablePath = child->FirstChildElement("programpath")->GetText();
			sp.path = child->FirstChildElement("path")->GetText();
			sp.filenameFormat = child->FirstChildElement("format")->GetText();

			sp.useDefaultImage = stob(child->FirstChildElement("usedefaultimage")->GetText());
			sp.useDefaultDate = stob(child->FirstChildElement("usedefaultdate")->GetText());
			sp.useDefaultFTP = stob(child->FirstChildElement("usedefaultftp")->GetText());

			if(sp.useDefaultImage)
				sp.image = this->image;
			else
			{
				TiXmlElement* child_img = child->FirstChildElement("image");
				sp.image.unit = child_img->Attribute("unit");
				sp.image.fileType = child_img->Attribute("filetype");
				sp.image.resize = stob(child_img->Attribute("resize"));
				sp.image.quality = stoi(child_img->Attribute("quality"));
				sp.image.resolution = stoi(child_img->Attribute("resolution"));

				TiXmlElement* width = child_img->FirstChildElement("width");
				sp.image.width = stof(width->GetText());
				TiXmlElement* height = child_img->FirstChildElement("height");
				sp.image.height = stof(height->GetText());
			}

			if(sp.useDefaultDate)
				sp.date = this->date;
			else
			{
				TiXmlElement* child_date = child->FirstChildElement("date");
				sp.date.enable = stob(child_date->Attribute("enable"));
				sp.date.xPos = stoi(child->Attribute("xpos"));
				sp.date.yPos = stoi(child->Attribute("ypos"));

				sp.date.textformat = child_date->FirstChildElement("textformat")->GetText();
				sp.date.opacity = stoi(child_date->FirstChildElement("opacity")->GetText());

				TiXmlElement* font = child_date->FirstChildElement("font");
				sp.date.fontName = font->GetText();
				sp.date.fontSize = stoi(font->Attribute("size"));
				sp.date.fontStyle = stoi(font->Attribute("style"));
				string color = font->Attribute("color");
				sp.date.fontColor[0] = stoi(color.substr(0, color.find_first_of("|")).substr(0, 3));
				sp.date.fontColor[0] = sp.date.fontColor[0] > 255 ? 255 : sp.date.fontColor[0];
				sp.date.fontColor[1] = stoi(color.substr(color.find_first_of("|") + 1, color.find_last_of("|") - 4).substr(0, 3));
				sp.date.fontColor[1] = sp.date.fontColor[1] > 255 ? 255 : sp.date.fontColor[1];
				sp.date.fontColor[2] = stoi(color.substr(color.find_last_of("|") + 1, 3));
				sp.date.fontColor[2] = sp.date.fontColor[2] > 255 ? 255 : sp.date.fontColor[2];
			}

			if(sp.useDefaultFTP)
				sp.ftp = this->ftp;
			else
			{
				TiXmlElement* child_ftp = child->FirstChildElement("ftp");
				sp.ftp.enable = stob(child_ftp->Attribute("enable"));
				sp.ftp.autosend = stob(child_ftp->Attribute("autosend"));
				TiXmlElement* server = child_ftp->FirstChildElement("server");
				sp.ftp.server = server->GetText() != 0 ? server->GetText() : "";
				sp.ftp.username = server->Attribute("username");
				sp.ftp.password = ::FTP::decrypt(server->Attribute("password"));
				sp.ftp.port = stoi(server->Attribute("port"));
				sp.ftp.path = child_ftp->FirstChildElement("path")->GetText() != 0 ? f->FirstChildElement("path")->GetText() : "";
			}

			this->specialPrograms[sp.name] = sp;
		}

		return true;
	}
	catch(...)
	{
		return false;
	}
	}
}
#endif