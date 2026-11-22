#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

struct BmpImage
{
	std::vector<std::uint8_t> bytes;

	std::size_t rowStride = 0;
	std::int32_t width = 0;
	std::int32_t height = 0;
	std::uint32_t pixelOffset = 0;

	bool loadImage(const std::string& filename);
	bool saveImage(const std::string& filename) const;
};
