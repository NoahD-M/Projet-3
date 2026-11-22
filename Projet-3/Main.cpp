#include <iostream>
#include <string>
#include "Bmp.h"
#include "CommandLine.h"

int main(int argc, char* argv[])
{
    CommandLineArguments args;
    std::string error;

    if (!CommandLineParse(argc, argv, args, error))
    {
        std::cerr << error;
        return 1;
    }

    if (!CommandLineVerify(args, error))
    {
        std::cerr << error;
        return 1;
    }

    CommandLineExecute(args, error);
    if (!error.empty())
    {
        std::cerr << error;
        return 1;
    }

    return 0;
}