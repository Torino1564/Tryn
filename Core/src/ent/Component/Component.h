#pragma once
#include <string>
#include <Core/src/utl/UUID.h>
#include <concepts>

#define DECLARE_UUID public: const static inline std::string UUID = tryn::utl::UUIDGenerator::Generate()
#define DECLARE_SRD(x) public: struct SubresourceData { bool active = false; x };

namespace tryn::ent
{
	template <typename T>
	concept ImplementsUUID = requires
	{
		std::convertible_to<decltype(T::UUID), std::string>;
	};

	template <typename T>
	bool HelperFunc()
	{
		return true;
	}

	template <typename T>
	concept ImplementsSRD = requires
	{
		typename T::SubresourceData;
	};

	template <typename T>
	concept Component = ImplementsSRD<T> && ImplementsUUID<T>;
}