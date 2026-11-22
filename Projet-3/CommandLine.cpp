#include "CommandLine.h"
#include "Bmp.h"
#include "Stegano.h"
#include <iostream>
#include <fstream>

bool CommandLineParse(int argc, char* argv[], CommandLineArguments& args, std::string& error)
{
	if (argc < 2)
	{
		error = "Missing command\n";
		return false;
	}

	std::string command = argv[1];

	if (command == "help")
	{
		args.option = CommandLineOption::HELP;
		return true;
	}
	else if (command == "encode")
	{
		if (argc != 5 && argc != 6)
		{
			error = "Invalid encode arguments\n";
			return false;
		}

		args.option = CommandLineOption::ENCODE_FILE;
		args.source = argv[2];
		args.destination = argv[3];
		args.message = argv[4];

		if (argc == 6)
		{
			args.key = argv[5];
			if (args.key.empty())
			{
				error = "Invalid encode arguments\n";
				return false;
			}
		}
	}
	else if (command == "decode")
	{
		if (argc != 3 && argc != 4)
		{
			error = "Invalid decode arguments\n";
			return false;
		}

		args.option = CommandLineOption::DECODE_FILE;
		args.source = argv[2];

		if (argc == 4)
		{
			args.key = argv[3];
			if (args.key.empty())
			{
				error = "Invalid decode arguments\n";
				return false;
			}
		}
	}
	else
	{
		error = "Unknown command\n";
		return false;
	}

	return true;
}

bool CommandLineVerify(const CommandLineArguments& args, std::string& error)
{
	if (args.option == CommandLineOption::ENCODE_FILE)
	{
		if (args.source.empty())
		{
			error = "Source file is required for encoding\n";
			return false;
		}
		if (args.destination.empty())
		{
			error = "Destination file is required for encoding\n";
			return false;
		}
		if (args.message.empty())
		{
			error = "Message is required for encoding\n";
			return false;
		}
		if (args.source == args.destination)
		{
			error = "Destination file cannot be the same as source file\n";
			return false;
		}

		std::ifstream existingFile(args.destination, std::ios::binary);
		if (existingFile.is_open())
		{
			existingFile.close();
			error = "Destination file already exists\n";
			return false;
		}
	}
	else if (args.option == CommandLineOption::DECODE_FILE)
	{
		if (args.source.empty())
		{
			error = "Source file is required for decoding\n";
			return false;
		}
	}

	return true;
}

void CommandLineExecute(const CommandLineArguments& args, std::string& error)
{
	if (args.option == CommandLineOption::HELP)
	{
		std::cout << "Commandes :\n";
		std::cout << "  Projet-3.exe help\n";
		std::cout << "  Projet-3.exe encode <source.bmp> <destination.bmp> \"<message>\" [cle]\n";
		std::cout << "  Projet-3.exe decode <source.bmp> [cle]\n\n";
		std::cout << "Exemples :\n";
		std::cout << "  Projet-3.exe encode input.bmp output.bmp \"Bonjour tout le monde\"\n";
		std::cout << "  Projet-3.exe encode input.bmp output.bmp \"Message secret avec espaces\" \"ma cle\"\n";
		std::cout << "  Projet-3.exe decode output.bmp\n";
		std::cout << "  Projet-3.exe decode output.bmp \"ma cle\"\n";
		return;
	}
	else if (args.option == CommandLineOption::ENCODE_FILE)
	{
		BmpImage image;
		if (!image.loadImage(args.source))
		{
			error = "Cannot open or invalid BMP file\n";
			return;
		}

		if (!encodeMessage(image, args.message, args.key, error))
		{
			return;
		}

		if (!image.saveImage(args.destination))
		{
			error = "Cannot save destination BMP file\n";
			return;
		}

		std::cout << "Message encoded in " << args.destination << '\n';
	}
	else if (args.option == CommandLineOption::DECODE_FILE)
	{
		BmpImage image;
		if (!image.loadImage(args.source))
		{
			error = "Cannot open or invalid BMP file\n";
			return;
		}

		std::string decodedMessage;
		if (!decodeMessage(image, args.key, decodedMessage, error))
		{
			return;
		}

		std::cout << decodedMessage << '\n';
	}
}
