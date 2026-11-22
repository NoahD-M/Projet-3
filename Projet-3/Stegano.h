#pragma once
#include <string>
#include <cstddef>
#include "Bmp.h"

std::size_t getMaxCapacity(const BmpImage& image);
bool encodeMessage(BmpImage& image, const std::string& message, const std::string& key, std::string& error);
bool decodeMessage(const BmpImage& image, const std::string& key, std::string& outMessage, std::string& error);
