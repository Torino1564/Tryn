#include "TrynPCH.h"
#include "DefaultTypeSerializers.h"
#include <iomanip>

namespace tryn::ser
{
	void Serialize(const StreamWriter& streamWriter, const std::string* pData, const bool binary,
		const std::string& name)
	{
		streamWriter.GetStringStream() << std::setw(sizeof(std::size_t) * 2)
			<< std::setfill('0')
			<< std::hex
			<< pData->size();

		streamWriter.GetStringStream() << *pData;
	}

	void Serialize(const StreamReader& sr, std::string* pData, const bool binary, const std::string& name)
	{
		std::size_t numChars = 0;

		char sizeStr[(sizeof(std::size_t) * 2) + 1] = { 0 };
		sr.GetStringStream().read(sizeStr, sizeof(std::size_t) * 2);

		numChars = std::strtoul(sizeStr, nullptr, 16);

		pData->resize(numChars);

		sr.GetStringStream().read(pData->data(), numChars);
	}
}
