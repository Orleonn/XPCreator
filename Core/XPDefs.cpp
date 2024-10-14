#include "XPDefs.hpp"

#include <fstream>



std::wstring XPReadFromFile(const std::wstring& filePath)
{
	std::wifstream file(filePath, std::ios::binary);
	if (!file.is_open())
		return {};

	std::size_t fileSize = 0;
	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (fileSize == 0)
		return {};

	std::wstring result(fileSize, L'\0');
	file.read(result.data(), fileSize);
	file.close();
	return result;
}
