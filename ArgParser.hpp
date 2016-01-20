#pragma once

#include <string>
#include <map>

class ArgParser
{
	public:
		ArgParser();

		void add(const std::string& argname, bool value);
		void add(const std::string& argname, int value);
		//void add(const std::string& argname, float value);
		void add(const std::string& argname, double value);
		//void add(const std::string& argname, long double value);
		void add(const std::string& argname, const char* value);
		void add(const std::string& argname, const std::string& value);
		void parse(int argc, char** argv);

		bool get_bool(const std::string& argname);
		int get_int(const std::string& argname);
		unsigned int get_uint(const std::string& argname);
		double get_double(const std::string& argname);
		const std::string get_string(const std::string& argname);

	private:
		std::map<std::string, bool> flags_default;
		std::map<std::string, bool> flags;
		std::map<std::string, int> ints;
		std::map<std::string, double> doubles;
		std::map<std::string, std::string> strings;
};