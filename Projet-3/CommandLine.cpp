#define _CRT_SECURE_NO_WARNINGS
#include "CommandLine.h"


bool CommandLineParse(int argc, char* argv[], CommandLineArguments& args, std::string& error) 
{
    if (argc < 2)
    {
        error = "Missing command\n";
        return false;
    }

    std::string command = argv[1];

    if (command == "encode")
    {
        if (argc != 5)
        {
            error = "Invalid encode arguments\n";
            return false;
        }
    }
    else if (command == "decode")
    {
        if (argc != 3)
        {
            error = "Invalid decode arguments\n";
            return false;
        }
    }
    else
    {
        error = "Unknown command\n";
		return false;
    }
	return true;
}

bool CommandLineVerify(const CommandLineArguments& args, std::string& error) {
	if (args.option == CommandLineOption::ENCODE_FILE) {
		if (args.source.empty()) {
			error = "Source file is required for encoding\n";
			return false;
		}
		if (args.destination.empty()) {
			error = "Destination file is required for encoding\n";
			return false;
		}
		if (args.message.empty()) {
			error = "Message is required for encoding\n";
			return false;
		}
	}
	else if (args.option == CommandLineOption::DECODE_FILE) {
		if (args.source.empty()) {
			error = "Source file is required for decoding\n";
			return false;
		}
	}
	return true;
}

void CommandLineExecute(const CommandLineArguments& args, std::string& error) {
	if (args.option == CommandLineOption::ENCODE_FILE) {
		// Call the function to encode the message into the source file and save it to the destination file
		return;
	}
	else if (args.option == CommandLineOption::DECODE_FILE) {
		// Call the function to decode the message from the source file
		return;
	}
}