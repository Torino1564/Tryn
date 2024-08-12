#pragma once
//#include <unordered_map>
#include <string>
#include <memory>
#include <Core/src/utl/Assert.h>
#include <concepts>
#include <span>
#include <array>
#include <Core/third/dynamic_bitset.hpp>
#include <type_traits>
#include <optional>
#include <Core/src/mem/ArenaAllocator.h>
#include <ranges>
#include <Core/src/mem/NativeArray.h>
#include <Core/src/utl/Span.h>
#include <iostream>
#include <Core/src/utl/StatefulMeta/CTC.h>
#include <Core/src/utl/StatefulMeta/CTM.h>
#include <Core/src/utl/Exception.h>
#include <Core/src/ecs/cmp/PrintMember.h>
#include <Core/src/utl/StringHasher.h>
#include "variant"
#include <Core/src/ecs/EntityID.h>
#include <Core/src/ecs/EcsClass.h>
#include <Core/src/ecs/Archetype_def.h>

#define ZT_COMPONENT_FIELDS(x) \
	public: struct SubresourceData{ x };\
	const static inline SubresourceData srd = {}

#define ZT_DEFINE_COMPONENT(x) class x; class x : public tryn::ecs::Component<x, #x>


#define ZT_DEFINE_COMPONENT_VARIABLE_2(type, var) \
	type var; \
	using ZT_CAT_C(var, _t) = tryn::utl::CTM::Map_t<type, #type, #var, sizeof(type), UUID>


#define ZT_DEFINE_COMPONENT_VARIABLE_3(type, var, initVal) \
	type var = initVal; \
	using ZT_CAT_C(var, _t) = tryn::utl::CTM::Map_t<type, #type, #var, sizeof(type), UUID>


#define GET_MACRO(_1,_2,_3,NAME,...) NAME
#define ZT_DEFINE_COMPONENT_VAR(...) GET_MACRO(__VA_ARGS__, ZT_DEFINE_COMPONENT_VARIABLE_3, ZT_DEFINE_COMPONENT_VARIABLE_2)(__VA_ARGS__)


ZT_EX_DEF(ComponentSMPException);

namespace tryn::ecs
{
	class IComponent;

	template <typename T>
	struct PrintMemberVariable
	{
		void operator()()
		{
			typename T::TypeName_t typeNameFunc;
			typename T::VarName_t varNameFunc;
			typename T::VarSize_t varSizeFunc;
			typename T::ElementNumber_t elNumberFunc;
			std::cout << std::format("Element Number: {}\n Type Name: {}\n Var Name: {}\n Var Size: {}\n\n", elNumberFunc(), typeNameFunc(), varNameFunc(), varSizeFunc());
		}
	};

	class ComponentManager
	{
	private:
		ComponentManager() = default;
		template <typename T>
		static unsigned int RegisterComponent();
		static unsigned int NextFreeAndIncrement()
		{
			static unsigned int componentCount = 0;
			return componentCount++;
		}
	// stateful meta bs
		template <typename T, utl::StaticString Name>
		friend class Component;
		static constexpr std::uint16_t listID = 0;

	public:
		static auto& ComponentMap()
		{
			static std::unordered_map<utl::UUID_t, std::pair<std::unique_ptr<IComponent>, unsigned int>> componentMap;
			return componentMap;
		}
		static auto& ComponentVector()
		{
			static std::vector<utl::UUID_t> componentVector;
			return componentVector;
		}
		static ComponentManager& Get()
		{
			static ComponentManager singleton;
			return singleton;
		}
		static constexpr auto GetComponentListID()
		{
			return listID;
		}

		void ActivateComponent(std::uint16_t componentUUID, std::uint16_t componentIndex);	

		static size_t GetComponentCount()
		{
			return ComponentMap().size();
		}

		template <typename MapElement_t, ValidComponent C>
		struct DoNothing
		{
			constexpr void operator()()
			{
				return;
			}
		};

		template <
			template <typename, ValidComponent> typename Func = DoNothing,
			unsigned ComponentN = 0,
			bool FoundCmp = false,
			typename Component = int, 
			unsigned ElementN = 0,
			typename... FuncArgs,
			auto Tag = []{}>
		static void IterateComponentMembers(const utl::UUID_t componentUUID, FuncArgs&&... funcArgs );
	};

	class IComponent
	{
	public:
		virtual ~IComponent() = default;
		constexpr virtual utl::UUID_t GetUUID() const = 0;
		virtual const char* Name() const = 0;
		virtual size_t Size() const = 0;
		virtual void ConstructSRD(void* pData, size_t = 0) const = 0;
		virtual void DestroySRD(void* pData, size_t = 0) const = 0;
	};

	template <typename T, utl::StaticString Name_>
	class Component : public IComponent
	{
	public:
		ZT_COMPONENT_FIELDS();

	protected:
		using Name_t = decltype(utl::TextType<Name_>);
		static constexpr Name_t nameFunc;

	public:
		constexpr static inline const char* name = nameFunc();
		static constexpr auto UUID = ZT_STRING_HASH(nameFunc());
		static inline auto index = ComponentManager::RegisterComponent<T>();
		using ComponentType = T;
		constexpr utl::UUID_t GetUUID() const override
		{
			return UUID;
		}
		const char* Name() const override
		{
			return name;
		}
		size_t Size() const override
		{
			return sizeof(typename T::srd);
		}
		void ConstructSRD(void* pData, const size_t size) const override
		{
			// assert size if possible
			trynass(size == 0 || size == sizeof(T)).msg(L"Error constructing Subresource Data in place!");

			auto pData_ = static_cast<typename T::SubresourceData*>(pData);
			new (pData_) typename T::SubresourceData();
		}
		void DestroySRD(void* pData, const size_t size) const override
		{
			// assert size if possible
			trynass(size == 0 || size == sizeof(T)).msg(L"Error constructing Subresource Data in place!");

			auto pData_ = static_cast<typename T::SubresourceData*>(pData);
			std::destroy_at(pData);
		}
	private:
		utl::CTM::setter<0, std::tuple<>, utl::CTM::tu_tag, UUID> setter;
		using VarMap = utl::CTM::get_list<UUID>;
		static constexpr auto inline counted = utl::ctc::counter<T, ComponentManager::listID>;

	public:
		template <template <typename, ValidComponent> class Func, unsigned ElementN = 0, typename... FuncArgs>
		void IterateMembers(FuncArgs&&... funcArgs)
		{
			if constexpr (ElementN < std::tuple_size_v<VarMap>)
			{
				using MapElement = std::tuple_element_t<ElementN, VarMap>;
				Func<MapElement, Component> func;
				func(funcArgs...);
				IterateMembers<Func, ElementN + 1>(funcArgs...);
			}
		}
	};

	ZT_DEFINE_COMPONENT(ActivationComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(bool, activation);
		);
	};

	enum class Action
	{
		Fill, Delete
	};

	inline void ComponentData(const std::byte* pData, const utl::UUID_t componentUUID, const Action action);

	enum class ComponentInfo
	{
		Name, Size, Index
	};

	template <ValidComponent C>
	static constexpr const char* ComponentName_()
	{
		return C::name;
	}

	template <ValidComponent C>
	static constexpr std::size_t ComponentSize_()
	{
		return sizeof(typename C::SubresourceData);
	}

	template <ComponentInfo Info>
	struct ReturnType
	{
		using T = std::size_t;
	};

	template <> struct ReturnType<ComponentInfo::Name>
	{
		using T = const char*;
	};

	template <> struct ReturnType<ComponentInfo::Index>
	{
		using T = unsigned int;
	};

	template <ComponentInfo Info>
	typename ReturnType<Info>::T GetComponentInfo(const utl::UUID_t componentUUID);
}
