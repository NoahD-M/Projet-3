#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include<filesystem>
#include <stdio.h>

int main()
{
    std::cout
        << "Working Directory : "
        << std::filesystem::current_path()
        << '\n';

    size_t file_size = fopen("test.images/Apple.bmp", "rb");
   
        if (file_size) {
            std::cout << "File exists" << std::endl;
			fclose(fopen("test.images/Apple.bmp", "rb"));
        }
        else {
            std::cout << "File does not exist" << std::endl;
        }

    
}