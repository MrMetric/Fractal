#include "ArgParser.hpp"
#include <exception>
#include <sstream>
//#include <iostream>

ArgParser::ArgParser()
{
	//ctor
}

void ArgParser::add(const std::string& argname, bool value)
{
	//std::cout << "Default for " << argname << " (bool): " << value << "\n";
	this->flags_default[argname] = value;
	this->flags[argname] = value;
}

void ArgParser::add(const std::string& argname, int value)
{
	//std::cout << "Default for " << argname << " (int): " << value << "\n";
	this->ints[argname] = value;
}

void ArgParser::add(const std::string& argname, double value)
{
	//std::cout << "Default for " << argname << " (double): " << value << "\n";
	this->doubles[argname] = value;
}

void ArgParser::add(const std::string& argname, const char* value)
{
	this->add(argname, std::string(value));
}

void ArgParser::add(const std::string& argname, const std::string& value)
{
	//std::cout << "Default for " << argname << " (string): " << value << "\n";
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
			//std::cout << argument << " flag is " << this->flags[argument] << "\n";
			continue;
		}

		++arg;
		std::string value;
		try
		{
			value = std::string(argv[arg]);
		}
		catch(...)
		{
			throw std::runtime_error("No value given for " + argument);
		}

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

		throw std::runtime_error("Invalid argument: " + argument);
	}
}

const bool ArgParser::get_bool(const std::string& argname)
{
	//std::cout << "Got " << argname << " (bool): " << this->flags[argname] << "\n";
	return this->flags[argname];
}

const int ArgParser::get_int(const std::string& argname)
{
	//std::cout << "Got " << argname << " (int): " << this->ints[argname] << "\n";
	return this->ints[argname];
}

const double ArgParser::get_double(const std::string& argname)
{
	//std::cout << "Got " << argname << " (double): " << this->doubles[argname] << "\n";
	return this->doubles[argname];
}

const std::string ArgParser::get_string(const std::string& argname)
{
	//std::cout << "Got " << argname << " (string): " << this->strings[argname] << "\n";
	return this->strings[argname];
}