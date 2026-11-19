#define _CRT_SECURE_NO_WARNINGS
#include "CommandLine.h"


bool CommandLineParse(int argc, char* argv[], CommandLineArguments& args, std::string& error) 
{
    if (argc < 2)
    {
        error = "Missing command\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "encode")
    {
        if (argc != 5)
        {
            error = "Invalid encode arguments\n";
            return 1;
        }
    }
    else if (command == "decode")
    {
        if (argc != 3)
        {
            error = "Invalid decode arguments\n";
            return 1;
        }
    }
    else
    {
        error = "Unknown command\n";
        return 1;
    }
}

bool CommandLineValidate(const CommandLineArguments& args, std::string& error) {
    
}

