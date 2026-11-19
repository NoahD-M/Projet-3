#include "Bmp.h"
#include<cstdio>
#include <cstring>
#include <cmath>

bool BmpImage::loadImage(const char* filename)
{
	if (!filename) return false;

	std::FILE* file = std::fopen(filename, "rb");
	if (!file) {
		return false;
	}

	std::fseek(file, 0, SEEK_END);
	long fileSize = std::ftell(file);
	std::fseek(file, 0, SEEK_SET);

	if (fileSize < 54) {
		std::fclose(file);
		return false;
	}

	bytes.resize(static_cast<std::size_t>(fileSize));
	std::size_t bytesRead = std::fread(bytes.data(), 1, bytes.size(), file);
	std::fclose(file);

	if (bytesRead != bytes.size()) {
		return false;
	}

	if (bytes[0] != 'B' || bytes[1] != 'M') {
		return false;
	}

	std::uint16_t bitsPerPixel = 0;
	std::memcpy(&pixelOffset, &bytes, sizeof(pixelOffset));
	std::memcpy(&width, &bytes, sizeof(width));
	std::memcpy(&height, &bytes, sizeof(height));
}

bool BmpImage::saveImage(const char* filename) const
{
	return false;
}
