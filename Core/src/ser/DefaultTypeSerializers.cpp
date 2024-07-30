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
}
