#pragma once
#include "ComponentsBase.h"
#include "Component.h"
#include "ModelComponent.h"
#include <concepts>

namespace tryn::ent
{
	template <typename T>
	concept ImplementsGetUID = requires (T t)
	{
		{t.GetCUID() } -> std::convertible_to<ComponentType>;
	};
	template <typename T>
	concept IsComponentType = std::is_base_of<Component, T>::value;

	template <typename T>
	concept ComponentWithUID = ImplementsGetUID<T> && IsComponentType<T>;

	template <ComponentType>
	struct ComponentEnumMap
	{
		using ComponentType = ent::Component;
	};

	template <>
	struct ComponentEnumMap<ComponentType::Model>
	{
		using ComponentType = ent::ModelComponent;
	};


}