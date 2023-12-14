#pragma once
#include <Core/third/uuid_v4/endianness.h>
#include <Core/third/uuid_v4/uuid_v4.h>
#include <random>

namespace tryn::utl
{
	class UUIDGenerator
	{
	public:
		static std::string Generate()
		{
			auto& singleton = Get();
			return singleton.gen.getUUID().str();
		}
	private:
		static UUIDGenerator& Get()
		{
			static UUIDGenerator singleton;
			return singleton;
		}

		UUIDv4::UUIDGenerator<std::mt19937_64> gen;
	};
}