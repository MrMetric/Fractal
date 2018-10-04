#pragma once

#include <string>
#include <unordered_map>

class ArgParser
{
public:
	ArgParser();

	void add(const std::string& name, bool);
	void add(const std::string& name, int);
	void add(const std::string& name, long double);
	void add(const std::string& name, const char*);
	void add(const std::string& name, std::string);
	void parse(int argc, char** argv);

	bool         get_bool  (const std::string& name) const;
	int          get_int   (const std::string& name) const;
	unsigned int get_uint  (const std::string& name) const;
	long double  get_lfloat(const std::string& name) const;
	std::string  get_string(const std::string& name) const;

private:
	std::unordered_map<std::string, bool       > flags_default;
	std::unordered_map<std::string, bool       > flags;
	std::unordered_map<std::string, int        > ints;
	std::unordered_map<std::string, long double> lfloats;
	std::unordered_map<std::string, std::string> strings;
};
