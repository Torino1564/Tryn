#include "DefaultTypeSerializers.h"

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

	std::string TypeSerializer<std::string>::Read(const StreamReader& streamReader, const bool binary)
	{
		std::size_t numChars = 0;
		streamReader.GetStringStream() >> std::hex >> numChars;

		std::string newString;
		newString.resize(numChars);

		streamReader.GetStringStream().read(newString.data(), numChars);

		return newString;
	}
}
