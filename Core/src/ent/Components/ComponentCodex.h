#pragma once
#include "ComponentsBase.h"
#include "Component.h"
#include "ModelComponent.h"
#include "PhysicsComponent.h"
#include "BehaviourComponent.h"
#include <concepts>

namespace tryn::ent
{
	template <typename T>
	concept ImplementsInterface = requires (T t)
	{
		{t.GetCUID() } -> std::convertible_to<ComponentType>;

		{
			T::Execute(std::declval<std::span<typename T::SubresourceData>>())
		} -> std::same_as<void>;
	};
	template <typename T>
	concept IsComponentType = std::is_base_of<Component, T>::value;

	template <typename T>
	concept CompleteSubresourceData = std::convertible_to<decltype(T::SubresourceData::active), bool>;

	template <typename T>
	concept ComponentWithUID = ImplementsInterface<T> && IsComponentType<T> ;

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

	template <>
	struct ComponentEnumMap<ComponentType::Behaviour>
	{
		using ComponentType = ent::BehaviourComponent;
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