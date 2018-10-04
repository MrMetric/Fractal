#include "ArgParser.hpp"

#include <stdexcept>
#include <string>
#include <utility>

using std::string;

ArgParser::ArgParser()
{
}

void ArgParser::add(const string& name, const bool value)
{
	this->flags_default.emplace(name, value);
	this->flags.emplace(name, value);
}

void ArgParser::add(const string& name, const int value)
{
	this->ints.emplace(name, value);
}

void ArgParser::add(const string& name, const long double value)
{
	this->lfloats.emplace(name, value);
}

void ArgParser::add(const string& name, const char* value)
{
	this->strings.emplace(name, value);
}

void ArgParser::add(const string& name, string value)
{
	this->strings.emplace(name, std::move(value));
}

void ArgParser::parse(const int argc, char** const argv)
{
	for(int arg = 1; arg < argc; ++arg)
	{
		const string argument = argv[arg];

		if(this->flags.find(argument) != this->flags.end())
		{
			this->flags.insert_or_assign(argument, !this->flags_default[argument]);
			continue;
		}

		++arg;
		if(argc <= arg)
		{
			throw std::runtime_error("No value given for " + argument);
		}
		const string value = argv[arg];

		if(this->ints.find(argument) != this->ints.end())
		{
			this->ints.insert_or_assign(argument, std::stoi(value));
			continue;
		}

		if(this->lfloats.find(argument) != this->lfloats.end())
		{
			this->lfloats.insert_or_assign(argument, std::stold(value));
			continue;
		}

		if(this->strings.find(argument) != this->strings.end())
		{
			this->strings.insert_or_assign(argument, value);
			continue;
		}

		throw std::runtime_error("Unknown argument: " + argument);
	}
}

bool ArgParser::get_bool(const string& name) const
{
	return this->flags.at(name);
}

int ArgParser::get_int(const string& name) const
{
	return this->ints.at(name);
}

unsigned int ArgParser::get_uint(const string& name) const
{
	const int i = this->ints.at(name);
	if(i < 0)
	{
		throw std::runtime_error("unsigned argument '" + name + "' is negative (" + std::to_string(i) + ")");
	}
	return static_cast<unsigned int>(i);
}

long double ArgParser::get_lfloat(const string& name) const
{
	return this->lfloats.at(name);
}

string ArgParser::get_string(const string& name) const
{
	return this->strings.at(name);
}
