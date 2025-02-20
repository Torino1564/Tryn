#include "TrynPCH.h"
#include "DefaultTypeSerializers.h"
#include <iomanip>

namespace tryn::ser
{
	void TypeSerializer<std::string>::Write(const StreamWriter& streamWriter, const std::string& data,const bool binary, const std::string& name)
	{
		streamWriter.GetStringStream()	<< std::setw(sizeof(std::size_t) * 2)
										<< std::setfill('0')
										<< std::hex
										<< data.size();

		streamWriter.GetStringStream() << data;
	}

	void TypeSerializer<std::string>::Read(std::string& data, const StreamReader& sr, const bool binary,
		const ExtraDataPack* pExtraData)
	{
		std::size_t numChars = 0;

		char sizeStr[(sizeof(std::size_t) * 2) + 1] = { 0 };
		sr.GetStringStream().read(sizeStr, sizeof(std::size_t) * 2);

		numChars = std::strtoul(sizeStr, nullptr, 16);

		data.resize(numChars);

		sr.GetStringStream().read(data.data(), numChars);
	}
}
