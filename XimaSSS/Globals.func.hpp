#ifndef _GLOBALS_FUNC_HPP
#define _GLOBALS_FUNC_HPP

__inline string itos(const int &i)
{
	std::stringstream buffer;
	buffer << i;
	return buffer.str();
}

__inline int stoi(const string &str)
{
	return atoi(str.c_str());
}

__inline string ftos(const float &f)
{
	std::stringstream buffer;
	buffer << f;
	return buffer.str();
}

__inline string dtos(const double &d)
{
	std::stringstream buffer;
	buffer << d;
	return buffer.str();
}

__inline float fast_atof_move_const(const char* c, float& out)
{
	static const float fast_atof_table[] = { 0.f, 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f, 0.000001f, 0.0000001f, 0.00000001f, 0.000000001f, 0.0000000001f, 0.00000000001f, 0.000000000001f, 0.0000000000001f, 0.00000000000001f, 0.000000000000001f };
	bool inv = false;
	char *t;
	if (*c=='-')
	{
		c++;
		inv = true;
	}
	out = (float)strtol(c, &t, 10);
	c = t;

	if (*c == '.')
	{
		c++;
		out += (float)strtol(c, &t, 10) * fast_atof_table[t-c];
		c = t;
		if (*c == 'e') 
		{ 
			++c;
			out *= powf(10.0f, (float)strtol(c, &t, 10)); 
			c = t; 
		}
	}
	if (inv)
		out *= -1.0f;
	return out;
}


__inline float stof(const string &str)
{
	float ret;
	return fast_atof_move_const(str.c_str(), ret);
}

__inline bool stob(const string &bval)
{
	// first pass, opt for explicit (true/1)/(false/0)
	if(bval == "true" || bval == "1")
		return true;
	else if(bval == "false" || bval == "0")
		return false;
	// second pass, check if the string is positive (not empty/NULL)
	if(stoi(bval) > 0)
		return true;
	else
		return false;
}

__inline string btos(const bool &bval)
{
	return string(bval == true ? "true" : "false");
}

__inline string ucfirst(string &str)
{
	ctype<char> const& ct = use_facet<ctype<char> >(locale());
	str[0] = ct.toupper(*str.begin());
	return str;
}

__inline int round(const double &x)
{
	return int(x > 0.0 ? x + 0.5 : x - 0.5);
}

__inline double round(const double &x, const int &precision)
{
	return round(x * pow(10, (double)precision)) / pow(10, (double)precision);
}

__inline float round(const float &x, const int &precision)
{
	return (float)round((double)x, precision);
}

__inline unsigned char isItalic(const int &style)
{
	return style == 2 || style == 4 ? 1 : 0;
}

__inline int getWeight(const int &style)
{
	return style == 3 || style == 4 ? 700 : 400;
}

__inline int makeStyle(const unsigned char &isItalic, const int &weight)
{
	if(isItalic)
	{
		if(weight == 400)
			return 2;
		else if(weight == 700)
			return 4;
		else
			return 0;
	}
	else
	{
		if(weight == 400)
			return 1;
		else if(weight == 700)
			return 3;
		else
			return 0;
	}
}
#endif