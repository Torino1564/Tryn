#pragma once
#include "ComponentsBase.h"
#include <concepts>

namespace tryn::ent
{
	template <typename T>
	concept ImplementsUID = requires (T t)
	{
		{t.GetUID() } -> std::convertible_to<ComponentType>;
	};

	template <typename T>
	concept CompleteSubresourceData = std::convertible_to<decltype(T::SubresourceData::active), bool>;

	template <typename T>
	struct FirstMemberOffset {
		static constexpr std::size_t value = offsetof(T, T::active);
	};

	template <typename T>
	concept BoolFirstMember = requires{
		{FirstMemberOffset<typename T::SubresourceData>::value == 0};
	};

	template <typename T>
	concept Component = ImplementsUID<T> && CompleteSubresourceData<T> && BoolFirstMember<T>;

	class EmptyComponent
	{
	public:
		static constexpr ComponentType GetUID()
		{
			return ComponentType::Unknown;	
		}
		struct SubresourceData
		{
			bool active = false;
		};
	};
}