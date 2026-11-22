#pragma once
#include <string>

std::string applyXor(const std::string& message, const std::string& key)
{
	if (key.empty())
	{
		return message;
	}

	std::string result = message;
	for (std::size_t i = 0; i < result.size(); ++i)
	{
		std::uint8_t messageByte = static_cast<std::uint8_t>(result[i]);
		std::uint8_t keyByte = static_cast<std::uint8_t>(key[i % key.size()]);
		std::uint8_t encrypted = messageByte ^ keyByte;
		result[i] = static_cast<char>(encrypted);
	}

	return result;
}

