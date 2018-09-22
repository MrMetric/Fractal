#include "ArgParser.hpp"
#include <exception>
#include <stdexcept>
#include <sstream>
//#include <iostream>

ArgParser::ArgParser()
{
	//ctor
}

void ArgParser::add(const std::string& argname, bool value)
{
	this->flags_default[argname] = value;
	this->flags[argname] = value;
}

void ArgParser::add(const std::string& argname, int value)
{
	this->ints[argname] = value;
}

void ArgParser::add(const std::string& argname, double value)
{
	this->doubles[argname] = value;
}

void ArgParser::add(const std::string& argname, const char* value)
{
	this->add(argname, std::string(value));
}

void ArgParser::add(const std::string& argname, const std::string& value)
{
	this->strings[argname] = value;
}

void ArgParser::parse(int argc, char** argv)
{
	for(int arg = 1; arg < argc; ++arg)
	{
		std::string argument(argv[arg]);

		if(this->flags.find(argument) != this->flags.end())
		{
			this->flags[argument] = !this->flags_default[argument];
			continue;
		}

		++arg;
		if(argc <= arg)
		{
			throw std::runtime_error("No value given for " + argument);
		}
		std::string value = argv[arg];

		if(this->ints.find(argument) != this->ints.end())
		{
			this->ints[argument] = atoi(value.c_str());
			continue;
		}

		if(this->doubles.find(argument) != this->doubles.end())
		{
			this->doubles[argument] = atof(value.c_str());
			continue;
		}

		if(this->strings.find(argument) != this->strings.end())
		{
			this->strings[argument] = value;
			continue;
		}

		throw std::runtime_error("Unknown argument: " + argument);
	}
}

bool ArgParser::get_bool(const std::string& argname)
{
	return this->flags[argname];
}

int ArgParser::get_int(const std::string& argname)
{
	return this->ints[argname];
}

unsigned int ArgParser::get_uint(const std::string& argname)
{
	int i = this->ints[argname];
	if(i < 0)
	{
		throw std::runtime_error("unsigned argument '" + argname + "' is negative (" + std::to_string(i) + ")");
	}
	return static_cast<unsigned int>(i);
}

double ArgParser::get_double(const std::string& argname)
{
	return this->doubles[argname];
}

const std::string ArgParser::get_string(const std::string& argname)
{
	return this->strings[argname];
}
