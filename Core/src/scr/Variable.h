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
	    void Serialize(class StreamIO& io, scr::Variable* data, bool binary, const std::string& name);
	}
}
