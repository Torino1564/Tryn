#pragma once
//#include <unordered_map>
#include <string>
#include <memory>
#include <Core/src/utl/Assert.h>
#include <concepts>
#include <span>
#include <array>
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
	const static inline SubresourceData srd = {};\
	static const unsigned int index;\
	static const std::vector<tryn::utl::CTM::ElementData>& GetReflectData_();\

#define ZT_DEFINE_COMPONENT(x) class x; class x : public tryn::ecs::Component<x>


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

	template <template <typename, ValidComponent Comp> class Callback, typename... Args>
	struct CallbackSignature
	{
		template <typename T, ValidComponent Comp> 
		using CallbackType = Callback<T, Comp>;

		using ArgTypes = std::tuple<Args...>;
	};

	
	template <typename T>
	concept CallbackSignatureClass = requires {
    typename T::ArgTypes;
	} && std::is_class_v<T>;


	template <typename MapElement_t, ValidComponent C>
	struct DoNothing
	{
		constexpr void operator()()
		{
			return;
		}
	};

	template <typename T, ValidComponent C>
	class VerySimpleCallback
	{
	public:
		void operator ()() const
		{
		}
	};

	class ComponentManager
	{
	private:
		ComponentManager() = default;
		static unsigned int NextFreeAndIncrement()
		{
			static unsigned int componentCount = 0;
			return componentCount++;
		}
	// stateful meta bs
		template <typename T>
		friend class Component;

		static constexpr std::uint16_t listID = 0;

	public:
		template <typename T>
		static unsigned int RegisterComponent();
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
		static auto& ComponentViewVector();

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
	};

	template <typename T, ValidComponent C>
	class FillElementData
	{
	public:
		void operator ()(std::vector<utl::CTM::ElementData>& vec) const
		{
			vec.push_back(utl::CTM::ElementData::MakeOffMapElement<T>());
		}
	};

	template <typename C>
	inline std::vector<utl::CTM::ElementData> CreateElementDataVector()
	{
		std::vector<utl::CTM::ElementData> retval;
		C::template IterateMembers<FillElementData>(retval);
		return retval;
	}

	class IComponent
	{
	public:
		virtual ~IComponent() = default;
		constexpr virtual utl::UUID_t GetUUID() const = 0;
		virtual const char* Name() const = 0;
		virtual size_t Size() const = 0;
		virtual void ConstructSRD(void* pData, size_t = 0) const = 0;
		virtual void DestroySRD(void* pData, size_t = 0) const = 0;
		virtual const std::vector<utl::CTM::ElementData>& GetReflectData() = 0;
	};

	template <typename T>
	class Component : public IComponent
	{
	public:
		const std::vector<utl::CTM::ElementData>& GetReflectData() override
		{
			return T::GetReflectData_();
		}
		static constexpr auto& tid = typeid(T);
		constexpr static auto name = tid.name();
		static constexpr auto UUID = ZT_STRING_HASH(name);
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

		template <typename C>
		friend std::vector<utl::CTM::ElementData> CreateElementDataVector();

		template <CallbackSignatureClass... Args>
		friend class ComponentView;
	private:
		utl::CTM::setter<0, std::tuple<>, utl::CTM::tu_tag, UUID> setter;
		using VarMap = utl::CTM::get_list<UUID>;

	protected:
		template <template <typename, ValidComponent> class Func = DoNothing, unsigned ElementN = 0, auto Tag = []{}, typename... FuncArgs>
		static void IterateMembers(FuncArgs&&... funcArgs);
	};

	template <CallbackSignatureClass... Signatures>
	class ComponentView
	{
		using SignatureTuple = std::tuple<Signatures...>;
	public:
		template <typename C>
		static ComponentView Make(const C& component)
		{
			ComponentView retval;
			retval.pComponent = &component;
			using Tuple0 = std::tuple_element_t<0, SignatureTuple>;
			using ArgsTuple0 = typename Tuple0::ArgTypes;

			using Indx = std::make_index_sequence<std::tuple_size_v<ArgsTuple0>>;

			retval.ImplementCallback<C, 0u, ArgsTuple0>(component, Indx{});

			return retval;
		}

		template <unsigned Index, typename... Args>
		void Func(Args&&... args) const
		{
			auto& funcPtr = std::get<Index>(funcPtrs);
			funcPtr(pComponent, std::forward_as_tuple(args...));
		}
	private:


		template <typename C, unsigned int N = 0, typename Tuple, std::size_t... Indices>
		void ImplementCallback(const C& component, std::index_sequence<Indices...>)
		{
			if constexpr (N < sizeof...(Signatures))
			{
				using Signature = std::tuple_element_t<N, SignatureTuple>;
				using ArgsTuple = typename Signature::ArgTypes;

				std::get<N>(funcPtrs) = [](const void* pComp, ArgsTuple&& args)
				{
					auto func = static_cast<const C*>(pComp)-> template IterateMembers<typename Signature::CallbackType, 0, []{}, std::tuple_element_t<Indices, Tuple>...>;
					std::apply(func, std::move(args));
				};

				if constexpr (N + 1< sizeof...(Signatures))
				{
					using TupleNp1 = std::tuple_element_t<N + 1, SignatureTuple>;
					using ArgsTupleNp1 = typename TupleNp1::ArgTypes;
					using Indx = std::make_index_sequence<std::tuple_size_v<ArgsTupleNp1>>;

					return ImplementCallback<C, N + 1, ArgsTupleNp1>(component, Indx{});
				}
			}
		}

		const void* pComponent = nullptr;
		std::tuple<void (*)(const void*, typename Signatures::ArgTypes&&)...> funcPtrs = {};
	};

	enum class Action
	{
		Fill, Delete
	};

	inline void ComponentData(const std::byte* pData, const utl::UUID_t componentUUID, const Action action)
	{
		auto& map = ComponentManager::ComponentMap();
		auto& [pComponent, index] = map[componentUUID];

		switch (action)
		{
		case Action::Fill:
			pComponent->ConstructSRD((void*)pData);
			break;
		case Action::Delete:
			pComponent->DestroySRD((void*)pData);
			break;
		}
	}

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
