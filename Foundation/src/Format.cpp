//
// Format.cpp
//
// $Id: //poco/svn/Foundation/src/Format.cpp#2 $
//
// Library: Foundation
// Package: Core
// Module:  Format
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Format.h"
#include "Poco/Exception.h"
#include <ios>
#include <sstream>
#include <cctype>
#include <cstddef>


namespace Poco {


namespace
{
	void parseFlags(std::ostream& str, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt)
	{
		bool isFlag = true;
		while (isFlag && itFmt != endFmt)
		{
			switch (*itFmt)
			{
			case '-': str.setf(std::ios::left); ++itFmt; break;
			case '+': str.setf(std::ios::showpos); ++itFmt; break;
			case '0': str.fill('0'); ++itFmt; break;
			case '#': str.setf(std::ios::showpoint | std::ios_base::showbase); ++itFmt; break;
			default:  isFlag = false; break;
			}
		}
	}


	void parseWidth(std::ostream& str, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt)
	{
		int width = 0;
		while (itFmt != endFmt && std::isdigit(*itFmt))
		{
			width = 10*width + *itFmt - '0';
			++itFmt;
		}
		if (width != 0) str.width(width);
	}
	
	
	void parsePrec(std::ostream& str, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt)
	{
		if (itFmt != endFmt && *itFmt == '.')
		{
			++itFmt;
			int prec = 0;
			while (itFmt != endFmt && std::isdigit(*itFmt))
			{
				prec = 10*prec + *itFmt - '0';
				++itFmt;
			}
			if (prec != 0) str.precision(prec);
		}
	}
	
	char parseMod(std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt)
	{
		char mod = 0;
		if (itFmt != endFmt)
		{
			switch (*itFmt)
			{
			case 'l':
			case 'h':
			case 'L': 
			case '?': mod = *itFmt++; break;
			}
		}
		return mod;
	}
	
	
	void prepareFormat(std::ostream& str, char type)
	{
		switch (type)
		{
		case 'd':
		case 'i': str << std::dec; break;
		case 'o': str << std::oct; break;
		case 'x': str << std::hex; break;
		case 'X': str << std::hex << std::uppercase; break;
		case 'e': str << std::scientific; break;
		case 'E': str << std::scientific << std::uppercase; break;
		case 'f': str << std::fixed; break;
		}
	}
	
	
	void writeAnyInt(std::ostream& str, const Any& any)
	{
		if (any.type() == typeid(char))
			str << static_cast<int>(AnyCast<char>(any));
		else if (any.type() == typeid(signed char))
			str << static_cast<int>(AnyCast<signed char>(any));
		else if (any.type() == typeid(unsigned char))
			str << static_cast<unsigned>(AnyCast<unsigned char>(any));
		else if (any.type() == typeid(short))
			str << AnyCast<short>(any);
		else if (any.type() == typeid(unsigned short))
			str << AnyCast<unsigned short>(any);
		else if (any.type() == typeid(int))
			str << AnyCast<int>(any);
		else if (any.type() == typeid(unsigned int))
			str << AnyCast<unsigned int>(any);
		else if (any.type() == typeid(long))
			str << AnyCast<long>(any);
		else if (any.type() == typeid(unsigned long))
			str << AnyCast<unsigned long>(any);
		else if (any.type() == typeid(Int64))
			str << AnyCast<Int64>(any);
		else if (any.type() == typeid(UInt64))
			str << AnyCast<UInt64>(any);
		else if (any.type() == typeid(bool))
			str << AnyCast<bool>(any);
	}


	void formatOne(std::string& result, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt, std::vector<Any>::const_iterator& itVal)
	{
		std::ostringstream str;
		parseFlags(str, itFmt, endFmt);
		parseWidth(str, itFmt, endFmt);
		parsePrec(str, itFmt, endFmt);
		char mod = parseMod(itFmt, endFmt);
		if (itFmt != endFmt)
		{
			char type = *itFmt++;
			prepareFormat(str, type);
			switch (type)
			{
			case 'b':
				str << AnyCast<bool>(*itVal++);
				break;
			case 'c':
				str << AnyCast<char>(*itVal++);
				break;
			case 'd':
			case 'i':
				switch (mod)
				{
				case 'l': str << AnyCast<long>(*itVal++); break;
				case 'L': str << AnyCast<Int64>(*itVal++); break;
				case 'h': str << AnyCast<short>(*itVal++); break;
				case '?': writeAnyInt(str, *itVal++); break;
				default:  str << AnyCast<int>(*itVal++); break;
				}
				break;
			case 'o':
			case 'u':
			case 'x':
			case 'X':
				switch (mod)
				{
				case 'l': str << AnyCast<unsigned long>(*itVal++); break;
				case 'L': str << AnyCast<UInt64>(*itVal++); break;
				case 'h': str << AnyCast<unsigned short>(*itVal++); break;
				case '?': writeAnyInt(str, *itVal++); break;
				default:  str << AnyCast<unsigned>(*itVal++); break;
				}
				break;
			case 'e':
			case 'E':
			case 'f':
				switch (mod)
				{
				case 'l': str << AnyCast<long double>(*itVal++); break;
				case 'L': str << AnyCast<long double>(*itVal++); break;
				case 'h': str << AnyCast<float>(*itVal++); break;
				default:  str << AnyCast<double>(*itVal++); break;
				}
				break;
			case 's':
				str << RefAnyCast<std::string>(*itVal++);
				break;
			case 'z':
				str << AnyCast<std::size_t>(*itVal++); 
				break;
			case 'I':
			case 'D':
			default:
				str << type;
			}
		}
		result.append(str.str());
	}
}


std::string format(const std::string& fmt, const Any& value)
{
	std::string result;
	format(result, fmt, value);
	return result;
}


std::string format(const std::string& fmt, const Any& value1, const Any& value2)
{
	std::string result;
	format(result, fmt, value1, value2);
	return result;
}


std::string format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3)
{
	std::string result;
	format(result, fmt, value1, value2, value3);
	return result;
}


std::string format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4)
{
	std::string result;
	format(result, fmt, value1, value2, value3, value4);
	return result;
}


std::string format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5)
{
	std::string result;
	format(result, fmt, value1, value2, value3, value4, value5);
	return result;
}


std::string format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5, const Any& value6)
{
	std::string result;
	format(result, fmt, value1, value2, value3, value4, value5, value6);
	return result;
}


void format(std::string& result, const std::string& fmt, const Any& value)
{
	std::vector<Any> args;
	args.push_back(value);
	format(result, fmt, args);
}


void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2)
{
	std::vector<Any> args;
	args.push_back(value1);
	args.push_back(value2);
	format(result, fmt, args);
}


void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3)
{
	std::vector<Any> args;
	args.push_back(value1);
	args.push_back(value2);
	args.push_back(value3);
	format(result, fmt, args);
}


void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4)
{
	std::vector<Any> args;
	args.push_back(value1);
	args.push_back(value2);
	args.push_back(value3);
	args.push_back(value4);
	format(result, fmt, args);
}


void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5)
{
	std::vector<Any> args;
	args.push_back(value1);
	args.push_back(value2);
	args.push_back(value3);
	args.push_back(value4);
	args.push_back(value5);
	format(result, fmt, args);
}


void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5, const Any& value6)
{
	std::vector<Any> args;
	args.push_back(value1);
	args.push_back(value2);
	args.push_back(value3);
	args.push_back(value4);
	args.push_back(value5);
	args.push_back(value6);
	format(result, fmt, args);
}


void format(std::string& result, const std::string& fmt, const std::vector<Any>& values)
{
	std::string::const_iterator itFmt  = fmt.begin();
	std::string::const_iterator endFmt = fmt.end();
	std::vector<Any>::const_iterator itVal  = values.begin();
	std::vector<Any>::const_iterator endVal = values.end(); 
	while (itFmt != endFmt)
	{
		switch (*itFmt)
		{
		case '%':
			++itFmt;
			if (itFmt != endFmt && itVal != endVal)
				formatOne(result, itFmt, endFmt, itVal);
			else if (itFmt != endFmt)
				result += *itFmt++;
			break;
		default:
			result += *itFmt;
			++itFmt;
		}
	}
}


} // namespace Poco
