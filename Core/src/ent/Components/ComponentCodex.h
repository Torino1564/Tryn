#pragma once
#include "ComponentsBase.h"
#include "Component.h"
#include "ModelComponent.h"
#include "PhysicsComponent.h"
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

	template <>
	struct ComponentEnumMap<ComponentType::Physics>
	{
		using ComponentType = ent::PhysicsComponent;
	};

	// Reverse
	template <ComponentWithUID C>
	struct ReverseComponentMap
	{
		static constexpr ComponentType type = ComponentType::Unknown;
	};

#define X(el) \
	using SysType##el = typename ComponentEnumMap<ComponentType::el>::ComponentType; \
	template <> struct ReverseComponentMap<SysType##el> \
	{ \
		static constexpr ComponentType type = ComponentType::el; \
	}; \

	COMPONENT_TYPES
#undef X
}