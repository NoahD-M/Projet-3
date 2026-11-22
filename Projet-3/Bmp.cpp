#include "Bmp.h"
#include <fstream>

std::uint16_t readUint16(const std::vector<std::uint8_t>& data, std::size_t offset)
{
	return static_cast<std::uint16_t>(
		data[offset] |
		(data[offset + 1] << 8)
		);
}

std::uint32_t readUint32(const std::vector<std::uint8_t>& data, std::size_t offset)
{
	return static_cast<std::uint32_t>(data[offset]) |
		(static_cast<std::uint32_t>(data[offset + 1]) << 8) |
		(static_cast<std::uint32_t>(data[offset + 2]) << 16) |
		(static_cast<std::uint32_t>(data[offset + 3]) << 24);
}

bool BmpImage::loadImage(const std::string& filename)
{
	if (filename.empty())
	{
		return false;
	}

	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (fileSize < 54)
	{
		file.close();
		return false;
	}

	bytes.resize(static_cast<std::size_t>(fileSize));
	file.read(reinterpret_cast<char*>(bytes.data()), bytes.size());
	std::streamsize bytesRead = file.gcount();
	file.close();

	if (static_cast<std::size_t>(bytesRead) != bytes.size())
	{
		return false;
	}

	if (bytes[0] != 'B' || bytes[1] != 'M')
	{
		return false;
	}

	pixelOffset = readUint32(bytes, 10);
	std::uint32_t dibHeaderSize = readUint32(bytes, 14);
	width = static_cast<std::int32_t>(readUint32(bytes, 18));
	height = static_cast<std::int32_t>(readUint32(bytes, 22));
	std::uint16_t planes = readUint16(bytes, 26);
	std::uint16_t bitsPerPixel = readUint16(bytes, 28);
	std::uint32_t compression = readUint32(bytes, 30);

	if (dibHeaderSize != 40 || planes != 1 || bitsPerPixel != 24 || compression != 0)
	{
		return false;
	}

	if (width <= 0 || height <= 0 || pixelOffset < 54)
	{
		return false;
	}

	rowStride = (static_cast<std::size_t>(width) * 3 + 3) / 4 * 4;

	if (static_cast<std::size_t>(pixelOffset) + static_cast<std::size_t>(height) * rowStride > bytes.size())
	{
		return false;
	}

	return true;
}

bool BmpImage::saveImage(const std::string& filename) const
{
	if (filename.empty() || bytes.empty())
	{
		return false;
	}

	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
	bool ok = file.good();
	file.close();

	return ok;
}
