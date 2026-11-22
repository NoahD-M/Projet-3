#pragma once
#include <string>
#include "Bmp.h"

bool SteganoEncode(BmpImage& image, const std::string& message);
bool SteganoDecode(const BmpImage& image, std::string& message);

