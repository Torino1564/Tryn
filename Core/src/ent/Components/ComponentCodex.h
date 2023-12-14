#pragma once
#include "Component.h"
#include "ModelComponent.h"
#include "PhysicsComponent.h"
#include "BehaviourComponent.h"
#include <concepts>

namespace tryn::ent
{
	template <ComponentType>
	struct ComponentEnumMap
	{
		using Type = ent::EmptyComponent;
	};

#define X(el)\
	template <> struct ComponentEnumMap<ComponentType::el> \
	{\
		using Type = ent::el##Component;\
	};
	COMPONENT_TYPES
#undef X

	// Reverse
	template <Component C>
	struct ReverseComponentMap
	{
		static constexpr ComponentType type = ComponentType::Unknown;
	};

#define X(el) \
	using SysType##el = typename ComponentEnumMap<ComponentType::el>::Type; \
	template <> struct ReverseComponentMap<SysType##el> \
	{ \
		static constexpr ComponentType type = ComponentType::el; \
	};
	using SysTypeModel = typename ComponentEnumMap<ComponentType::Model>::Type;
	template <> struct ReverseComponentMap<SysTypeModel>
	{
		static constexpr ComponentType type = ComponentType::Model;
	};
	using SysTypeBehaviour = typename ComponentEnumMap<ComponentType::Behaviour>::Type; template <> struct ReverseComponentMap<SysTypeBehaviour> {
		static constexpr ComponentType type = ComponentType::Behaviour;
	}; using SysTypePhysics = typename ComponentEnumMap<ComponentType::Physics>::Type; template <> struct ReverseComponentMap<SysTypePhysics> {
		static constexpr ComponentType type = ComponentType::Physics;
	};
#undef X
}