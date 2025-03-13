#pragma once
#include <any>
#include "Core/src/utl/StringHasher.h"

namespace tryn
{
	namespace scr
	{
		struct Variable
		{
			std::any var;
			utl::UUID_t uuid;
			std::string name;
			std::string typeName;
		};
	}

	namespace ser
	{
	    void SerializeWrite(const class StreamWriter& sw, const scr::Variable& data, const bool binary, const std::string& name);

	    void SerializeRead(const class StreamReader& sr, scr::Variable& data, const bool binary, class ExtraDataPack* pExtraData = nullptr);
	}
}
