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
	std::memcpy(&pixelOffset, bytes.data() +10, sizeof(pixelOffset));
	std::memcpy(&width, bytes.data() + 18, sizeof(width));
	std::memcpy(&height, bytes.data() + 22, sizeof(height));
	std::memcpy(&bitsPerPixel, bytes.data() + 28, sizeof(bitsPerPixel));

	if (width <= 0 || height == 0 || bitsPerPixel == 0) {
		return false;
	}

	rowStride = ((static_cast<std::size_t>(width) * bitsPerPixel + 31) / 32) * 4;
	std::size_t absHeight = static_cast<std::size_t>(std::llabs(static_cast<long long>(height)));

	if (static_cast<std::size_t>(pixelOffset) + absHeight * rowStride > bytes.size()) {
		return false;
	}

	return true;
}

bool BmpImage::saveImage(const char* filename) const
{
	if (!filename || bytes.empty()) {
		return false;
	}

	std::FILE* file = std::fopen(filename, "wb");
	if (!file) {
		return false;
	}

	std::size_t bytesWritten = std::fwrite(bytes.data(), 1, bytes.size(), file);
	std::fclose(file);

	return bytesWritten == bytes.size();
}
