#pragma once
#include <concepts>

namespace tryn::mem
{
	template <typename T>
	concept Allocator = requires (T t)
	{
		{t.Allocate(std::declval<std::uint32_t>)} -> std::convertible_to<void*>;
	};

}