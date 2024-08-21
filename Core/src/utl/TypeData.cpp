#include "TrynPCH.h"
#include "TypeData.h"

namespace tryn::utl
{
	void TypeRegister::Append(TypeData& newData)
	{
		// check for existence
		const auto it = uuidToType.find(newData.id);
		if (it == uuidToType.end())
		{
			// append new type
			uuidToType.insert({newData.id, newData});
			nameToType.insert({newData.name, newData});
		}
	}

	const TypeData& TypeRegister::Resolve(const std::string& name)
	{
		return nameToType[name];
	}

	const TypeData& TypeRegister::Resolve(UUID_t uuid)
	{
		return uuidToType[uuid];
	}

	TypeData::TypeData(const UUID_t id, std::string_view name, const size_t byteSize)
		: id(id), name(name), byteSize(byteSize)
	{
	}

	TypeData::TypeData(const TypeData& rhs): id(rhs.id), name(rhs.name), byteSize(rhs.byteSize)
	{}
}
