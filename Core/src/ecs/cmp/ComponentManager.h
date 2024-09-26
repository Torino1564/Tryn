#pragma once
#include <string>
#include <memory>
#include <Core/src/utl/Assert.h>
#include <concepts>
#include <array>
#include <type_traits>
#include <Core/src/mem/ArenaAllocator.h>
#include <Core/src/mem/NativeArray.h>
#include <Core/src/utl/Span.h>
#include <Core/src/utl/StatefulMeta/CTC.h>
#include <Core/src/utl/StatefulMeta/CTM.h>
#include <Core/src/utl/Exception.h>
#include <Core/src/ecs/cmp/PrintMember.h>
#include <Core/src/utl/StringHasher.h>
#include <Core/src/ecs/EntityID.h>
#include <Core/src/ecs/EcsClass.h>
#include <Core/src/ecs/Component.h>

#define ZT_COMPONENT_FIELDS(x) \
	public: struct SubresourceData{ x };\
	const static inline SubresourceData srd = {};\
	static const unsigned int index;\
	static const std::vector<tryn::utl::CTM::ElementData>& GetReflectData_();\

#define ZT_DEFINE_COMPONENT(x) class x; class x : public tryn::ecs::HardComponentWrapperBase<x>


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
	class IComponentWrapper;

	template <template <typename, ValidComponent Comp> class Callback, typename... Args>
	struct CallbackSignature
	{
		template <typename T, ValidComponent Comp>
		using CallbackType = Callback<T, Comp>;

		using ArgTypes = std::tuple<Args...>;
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
						auto func = static_cast<const C*>(pComp)-> template IterateMembers<typename Signature::CallbackType, 0, [] {}, std::tuple_element_t<Indices, Tuple>... > ;
						std::apply(func, std::move(args));
					};

				if constexpr (N + 1 < sizeof...(Signatures))
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

	class ComponentManager
	{
	public:

		ComponentManager(const ECS* pEcs);

		// Soft Components
		void RegisterComponent(class SoftComponentDescriptor&&);

		// Hard Components
		template <typename T>
		void RegisterComponent()
		{
			// assert uniqueness
			HardComponentWrapperBase<T> wrappedComponent;

			if (wrappedComponent.index == 0)
			{
				wrappedComponent.index = NextFreeAndIncrement();
			}

			componentWrappers.push_back(std::move(wrappedComponent));
		}

		uint32_t ComponentCount() const;

	private:
		uint32_t NextFreeAndIncrement();
		const ECS* pEcs = nullptr;
		uint32_t componentCount = 0;
		std::vector<IComponentWrapper> componentWrappers;
		
	};
}
