#include <iostream>
#include <fstream>
#include <string>
#include<filesystem>
#include <stdio.h>
#include"Bmp.h"
#include "CommandLine.h"

int main(int argc, char* argv[])
{
    std::cout                              //bmp.cpp
        << "Working Directory : "
        << std::filesystem::current_path()
        << '\n';

    std::string filePath = "test_images/Apple.bmp";
    FILE* bmpFile=fopen(filePath.c_str(), "rb");
   
        if (bmpFile) {
            std::cout << "File exists" << std::endl;
			fclose(bmpFile);
        }
        else {
            std::cout << filePath << " can not be opened" << std::endl;
        }

        return 0;
}