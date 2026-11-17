#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include<filesystem>
#include <stdio.h>



int main()
{
    std::cout                               //Arg.cpp
        << "Working Directory : "
        << std::filesystem::current_path()
        << '\n';

  FILE* bmpFile= fopen("test_images/Apple.bmp", "rb");
   
        if (bmpFile) {
            std::cout << "File exists" << std::endl;
			fclose(bmpFile);
        }
        else {
            std::cout << "File does not exist" << std::endl;
        }
}