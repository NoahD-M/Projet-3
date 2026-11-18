#include <iostream>
#include <fstream>
#include <string>
#include<filesystem>
#include <stdio.h>
#include"Bmp.h"

int main()
{
    std::cout                               //Arg.cpp
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
			;
            std::cout << filePath << " can not be opened" << std::endl;
        }
}