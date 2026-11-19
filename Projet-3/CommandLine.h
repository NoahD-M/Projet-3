#pragma once
#include <string>

//class CommandLine
//{
//};

enum class CommandLineOption
{
	HELP = 0,
	ENCODE_FILE,
	DECODE_FILE,
};

struct CommandLineArguments
{
	CommandLineOption option;
	std::string source;
	std::string destination;
	std::string message;
	std::string key;

};

bool CommandLineParse(int argc, char* argv[], CommandLineArguments& args, std::string& error);
bool CommandLineVerify(const CommandLineArguments& args, std::string& error);
bool CommandLineExecute(const CommandLineArguments& args, std::string& error);