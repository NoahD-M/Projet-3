#include "Stegano.h"
#include "XOR.h"
#include <cstdint>
#include <vector>

std::size_t getComponentIndex(const BmpImage& image, std::size_t bitIndex)
{
	std::size_t pixelIndex = bitIndex / 3;
	std::size_t channel = bitIndex % 3;
	std::size_t row = pixelIndex / static_cast<std::size_t>(image.width);
	std::size_t column = pixelIndex % static_cast<std::size_t>(image.width);

	std::size_t index =
		static_cast<std::size_t>(image.pixelOffset) +
		row * image.rowStride +
		column * 3 +
		channel;

	return index;
}

void writeByte(BmpImage& image, std::size_t byteIndex, std::uint8_t value)
{
	for (int bitPos = 7; bitPos >= 0; --bitPos)
	{
		std::uint8_t bit = (value >> bitPos) & 1u;
		std::size_t bitOffset = byteIndex * 8 + static_cast<std::size_t>(7 - bitPos);
		std::size_t index = getComponentIndex(image, bitOffset);
		std::uint8_t component = image.bytes[index];
		component = static_cast<std::uint8_t>((component & 0xFEu) | bit);
		image.bytes[index] = component;
	}
}

std::uint8_t readByte(const BmpImage& image, std::size_t byteIndex)
{
	std::uint8_t value = 0;
	for (int i = 0; i < 8; ++i)
	{
		std::size_t bitOffset = byteIndex * 8 + static_cast<std::size_t>(i);
		std::size_t index = getComponentIndex(image, bitOffset);
		std::uint8_t component = image.bytes[index];
		std::uint8_t bit = component & 1u;
		value = static_cast<std::uint8_t>((value << 1) | bit);
	}
	return value;
}

std::size_t getMaxCapacity(const BmpImage& image)
{
	if (image.width <= 0 || image.height <= 0)
	{
		return 0;
	}
	std::size_t totalBits = static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height) * 3;
	return totalBits / 8;
}

bool encodeMessage(BmpImage& image, const std::string& message, const std::string& key, std::string& error)
{
	std::size_t maxBytes = getMaxCapacity(image);
	std::size_t metadataSize = 9;

	if (maxBytes < metadataSize || message.size() > maxBytes - metadataSize)
	{
		error = "Message is too long\n";
		return false;
	}

	std::string textToHide = message;
	std::uint8_t xorFlag = 0;

	if (!key.empty())
	{
		textToHide = applyXor(message, key);
		xorFlag = 1;
	}

	std::uint32_t length = static_cast<std::uint32_t>(textToHide.size());

	std::vector<std::uint8_t> data;
	data.push_back('S');
	data.push_back('T');
	data.push_back('E');
	data.push_back('G');
	data.push_back(xorFlag);
	data.push_back(static_cast<std::uint8_t>(length & 0xFFu));
	data.push_back(static_cast<std::uint8_t>((length >> 8) & 0xFFu));
	data.push_back(static_cast<std::uint8_t>((length >> 16) & 0xFFu));
	data.push_back(static_cast<std::uint8_t>((length >> 24) & 0xFFu));

	for (std::size_t i = 0; i < textToHide.size(); ++i)
	{
		data.push_back(static_cast<std::uint8_t>(textToHide[i]));
	}

	for (std::size_t i = 0; i < data.size(); ++i)
	{
		writeByte(image, i, data[i]);
	}

	return true;
}

bool decodeMessage(const BmpImage& image, const std::string& key, std::string& outMessage, std::string& error)
{
	std::size_t maxBytes = getMaxCapacity(image);
	std::size_t metadataSize = 9;

	if (maxBytes < metadataSize)
	{
		error = "Image is too small to contain a message\n";
		return false;
	}

	if (readByte(image, 0) != 'S' ||
		readByte(image, 1) != 'T' ||
		readByte(image, 2) != 'E' ||
		readByte(image, 3) != 'G')
	{
		error = "No hidden message found in this image\n";
		return false;
	}

	std::uint8_t xorFlag = readByte(image, 4);
	if (xorFlag != 0 && xorFlag != 1)
	{
		error = "Invalid metadata in image\n";
		return false;
	}

	if (xorFlag == 1 && key.empty())
	{
		error = "A key is required to decode this message\n";
		return false;
	}

	if (xorFlag == 0 && !key.empty())
	{
		error = "This message was encoded without a key\n";
		return false;
	}

	std::uint32_t b0 = readByte(image, 5);
	std::uint32_t b1 = readByte(image, 6);
	std::uint32_t b2 = readByte(image, 7);
	std::uint32_t b3 = readByte(image, 8);
	std::uint32_t length = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);

	if (length == 0 || static_cast<std::size_t>(length) > maxBytes - metadataSize)
	{
		error = "Invalid message length\n";
		return false;
	}

	std::string extracted(length, '\0');
	for (std::uint32_t i = 0; i < length; ++i)
	{
		extracted[i] = static_cast<char>(readByte(image, metadataSize + i));
	}

	if (xorFlag == 1)
	{
		outMessage = applyXor(extracted, key);
	}
	else
	{
		outMessage = extracted;
	}

	return true;
}
