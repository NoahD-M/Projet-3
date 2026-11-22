#pragma once
#include <string>

enum class CommandLineOption
{
	HELP = 0,
	ENCODE_FILE,
	DECODE_FILE,
};

struct CommandLineArguments
{
	CommandLineOption option = CommandLineOption::HELP;
	std::string source;
	std::string destination;
	std::string message;
	std::string key;
};

bool CommandLineParse(int argc, char* argv[], CommandLineArguments& args, std::string& error);
bool CommandLineVerify(const CommandLineArguments& args, std::string& error);
void CommandLineExecute(const CommandLineArguments& args, std::string& error);
