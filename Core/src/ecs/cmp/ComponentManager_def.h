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
	public: struct SubresourceData{ x }

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
	public:
		static ComponentManager& Get()
		{
			static ComponentManager singleton;
			return singleton;
		}

		static constexpr auto GetComponentListID()
		{
			return listID;
		}

		template <typename C>
		int RegisterComponent();


		void ActivateComponent(std::uint16_t componentUUID, std::uint16_t componentIndex);	

		template <unsigned int N = 0, bool Index = true>
		static const char* GetComponentName(unsigned int componentUUID);

		template <unsigned int N = 0, bool Index = true>
		static std::size_t GetComponentSize(unsigned int componentUUID);

	private:
		ComponentManager();

		template <unsigned int N = 0>
		void RegisterComponents();

		std::uint16_t componentCounter = 0;
		//std::unordered_map<ComponentIndex, ComponentSize> map;

	// stateful meta bs
		template <typename T, utl::StaticString Name>
		friend class Component;
	private:
		static constexpr std::uint16_t listID = 0;
	public:
		template <auto Tag = []{}>
		using ComponentList = utl::ctc::get_list<listID>;

		template <auto Tag = []{}>
		static constexpr auto GetComponentCount();

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
			typename... FuncArgs>
		static void IterateComponentMembers(long long componentUUID, FuncArgs&&... funcArgs );

		template <unsigned N, auto Tag = []{}>
		using ComponentByIndex = typename std::remove_reference_t<decltype(std::get<N>(std::declval<ComponentList<>>()))>;
	};

	template <typename T, utl::StaticString Name>
	class Component
	{
	public:
		virtual ~Component() = default;
		ZT_COMPONENT_FIELDS();
	private:
		using Name_t = decltype(utl::TextType<Name>);
		static constexpr Name_t nameFunc;
	public:
		constexpr static inline const char* name = nameFunc();
		using ComponentType = T;
		static constexpr auto accessMode = AccessMode::ReadWrite;
		static constexpr auto inline index = utl::ctc::counter<T, ComponentManager::listID>;
		static constexpr auto UUID = ZT_STRING_HASH(nameFunc());
	private:
		utl::CTM::setter<0, std::tuple<>, utl::CTM::tu_tag, UUID> setter;
	};

	ZT_DEFINE_COMPONENT(ActivationComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(bool, activation);
		);
	};

	template <unsigned N = 0, auto Tag = []{}>
	static constexpr unsigned int GetComponentIndex(const utl::UUID_t componentUUID);

	template <typename T>
	void FillData(std::byte* pData);

	template <typename T>
	void DeleteData(std::byte* pData);

	enum class Action
	{
		Fill, Delete
	};

	template <Action Action, auto Tag = []{}>
	void ComponentData(std::byte* pData, const utl::UUID_t componentUUID);

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

	template <ValidComponent C>
	static constexpr unsigned int ComponentIndex_()
	{
		return C::index;
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

	template <ComponentInfo Info, auto Tag = []{}>
	typename ReturnType<Info>::T GetComponentInfo(const utl::UUID_t componentUUID);
}
