#pragma once
#include <unordered_map>
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

#include <Core/src/utl/StatefulMeta/CTC.h>

#define ZT_COMPONENT_FIELDS(x) \
	public: struct SubresourceData{ x }

#define ZT_DEFINE_COMPONENT(x) class x : public tryn::ecs::Component<x>

namespace tryn::ecs
{
	class ComponentManager;

	template <typename T>
	class Component;

	template <typename T>
	concept ImplementsUUID = requires
	{
		std::convertible_to<decltype(T::UUID), std::string>;
	};

	template <typename T>
	concept ImplementsSRD = requires
	{
		typename T::SubresourceData;
	};

	template <typename T>
	concept ValidComponent = ImplementsSRD<T> && ImplementsUUID<T>;

	enum class AccessMode
	{
		ReadWrite,
		ReadOnly,
		WriteOnly,
		Discard
	};

	template <typename T>
	concept ValidComponentWithAccessMode =  ValidComponent<typename T::ComponentType> and requires
	{
		{T::accessMode} -> std::convertible_to<AccessMode>;
	};


	template <ValidComponent C>
	struct ReadOnly
	{
		using ComponentType = C;
		static constexpr auto accessMode = AccessMode::ReadOnly;
	};

	template <ValidComponent C>
	struct ReadWrite
	{
		using ComponentType = C;
		static constexpr auto accessMode = AccessMode::ReadWrite;
	};

	template <ValidComponent C>
	struct WriteOnly
	{
		using ComponentType = C;
		static constexpr auto accessMode = AccessMode::WriteOnly;
	};

	template <ValidComponent C>
	struct Discard
	{
		using ComponentType = C;
		static constexpr auto accessMode = AccessMode::Discard;
	};

	class ComponentManager;
	class ArchetypeManager;

	using ComponentIndex = typename int;
	using ComponentSize = typename std::size_t;

	class ECS
	{
	public:
		static ECS& Get()
		{
			static ECS ecs;
			return ecs;
		}
		ComponentManager& componentManager;
		ArchetypeManager& archetypeManager;
		mem::ArenaAllocator<> allocator;
	private:
		ECS();
	};

	class ComponentManager
	{
	public:
		static ComponentManager& Get()
		{
			static ComponentManager singleton;
			return singleton;
		}

		template <typename C>
		int RegisterComponent()
		{
			map.insert({ componentCounter, sizeof(C::SubresourceData)});
			return componentCounter++;
		}

		auto& GetComponentMap()
		{
			return map;
		}

		void ActivateComponent(std::uint16_t componentUUID, std::uint16_t componentIndex);
	private:
		ComponentManager() = default;
		std::uint16_t componentCounter = 0;
		std::unordered_map<ComponentIndex, ComponentSize> map;

	// stateful meta bs
		template <typename T>
		friend class Component;
	private:
		static constexpr std::uint16_t listID = 0;
	public:
		template <auto Tag = []{}>
		using ComponentList = utl::ctc::get_list<listID>;

		template <auto Tag = []{}>
		static constexpr auto GetComponentCount()
		{
			return utl::ctc::element_count<listID>();
		}

		template <unsigned N, auto Tag = []{}>
		using ComponentByIndex = typename std::remove_reference_t<decltype(std::get<N>(std::declval<ComponentManager::ComponentList<>>()))>;
	};

	template <typename T>
	class Component
	{
	public:
		ZT_COMPONENT_FIELDS();
	public:
		constexpr static inline const char* name = "Default Component";
		using ComponentType = T;
		static constexpr auto accessMode = AccessMode::ReadWrite;
		const static inline int UUID = ComponentManager::Get().RegisterComponent<T>();
		static constexpr auto ctcID = utl::ctc::counter<T, ComponentManager::listID>;
	};

	ZT_DEFINE_COMPONENT(ActivationComponent)
	{
		ZT_COMPONENT_FIELDS();
	};
	
	template <ValidComponent... Cs>
	using HeterogeneusComponentDataPointerContainer = typename std::tuple<std::span<typename Cs::SubresourceData>...>;

	class Archetype
	{
		friend class Entity;
		friend class ArchetypeManager;

	public:
		template <ValidComponent... Cs>
		void FillComponentPointerTuple(std::tuple<std::span<typename Cs::SubresourceData>...>& container)
		{
			FillComponentPointerTupleImpl<0,Cs...>(container);
		}

		template <int N, typename... Cs> using NthTypeOf = 
			typename std::tuple_element<N, std::tuple<Cs...>>::type;

		template <int N = 0, ValidComponent... Cs>
		void FillComponentPointerTupleImpl(std::tuple<std::span<typename Cs::SubresourceData>...>& container)
		{
			std::get<N>(container) = GetComponentData<NthTypeOf<N, Cs...>>();
			if constexpr (N < sizeof...(Cs) - 1)
			{
				FillComponentPointerTupleImpl<N + 1, Cs...>(container);
			}
		}

		template <ValidComponent C>
		std::span<typename C::SubresourceData> GetComponentData()
		{
			for (auto [index, componentUUID] : std::ranges::views::enumerate(components))
			{
				if (componentUUID == C::UUID)
				{
					return std::span<typename C::SubresourceData>(
						reinterpret_cast<typename C::SubresourceData*>(bufferPtrs[index]->data()), upperLimit);
				}
			}
			return {};
		}

		template <ValidComponent... Cs>
		static Archetype Make()
		{
			Archetype archetype;
			archetype.InitializeUUID();
			archetype.AppendComponents<Cs...>();
			archetype.Resize(100);
			return archetype;
		}
		static Archetype Make(std::span<int> componentIDs)
		{
			Archetype archetype;
			archetype.InitializeUUID();
			archetype.components.reserve(componentIDs.size());
			for (auto i = 0 ; i < componentIDs.size() ; i++)
			{
				archetype.components.push_back(componentIDs[i]);
				archetype.bufferPtrs.push_back(std::make_unique<std::vector<std::byte>>());
			}
			archetype.Resize(100);
			return archetype;
		}
		template <ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void AppendComponents()
		{
			AppendComponents<First>();
			AppendComponents<Second, Rest...>();
		}
		template <ValidComponent C>
		void AppendComponents()
		{
			components.push_back(C::UUID);
			bufferPtrs.push_back(std::make_unique<std::vector<std::byte>>());
		}
		const int GetUUID() const
		{
			return this->UUID;
		}
		const auto ComponentCount() const
		{
			return components.size();
		}
		const auto ComponentArraySize() const
		{
			return upperLimit;
		}
		struct EntityID ResolveEntityUUID();
		void Free(struct EntityID);
		void Grow()
		{
			Resize((uint32_t)(booker.size() * 1.5f));
		}
		void Resize(std::uint32_t newSize)
		{
			booker.resize(newSize, true);
			for (auto [index, pBuffer]: std::ranges::views::enumerate(bufferPtrs) )
			{
				pBuffer->resize(newSize * ComponentManager::Get().GetComponentMap().at(components[index]));
			}
		}
	private:
		void InitializeUUID();
		std::uint16_t UUID = 0;
		std::vector<int> components;
		std::uint32_t bookerPointer = 0;
		std::uint32_t upperLimit = 0;
		sul::dynamic_bitset<> booker;
		std::vector<std::unique_ptr<std::vector<std::byte>>> bufferPtrs;
	};

	class ArchetypeManager
	{
	public:
		friend class Archetype;

	public:
		template <ValidComponentWithAccessMode... Cs>
		std::span<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>> GetComponentGroups()
		{
			auto archetypeQuery = QueryArchetype<typename Cs::ComponentType...>();
			mem::NativeArray<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>> heterogeneusComponentSpanArray((uint32_t)archetypeQuery.size(), ECS::Get().allocator);

			for (auto i = 0 ; i < archetypeQuery.size() ; i++)
			{
				archetypeQuery[i]->FillComponentPointerTuple<typename Cs::ComponentType...>(heterogeneusComponentSpanArray[i]);
			}
			
			return std::span<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>>(heterogeneusComponentSpanArray.begin(), heterogeneusComponentSpanArray.size());
		}

		std::span<Archetype*> QueryArchetype(std::span<ComponentIndex> componentIDs)
		{
			static std::vector<std::pair<Archetype*, int> > archetypeMap;
			static bool initialized = false;
			if (!initialized)
			{
				initialized = true;
				archetypeMap.resize(1000);
			}

			std::fill(archetypeMap.begin(), archetypeMap.end(), std::pair<Archetype*, int>{nullptr, 0});

			auto pResult = ECS::Get().allocator.MakeNew<std::array<Archetype*, 100>>();
			auto& result = *pResult;

			for (int i = 0 ; i < componentIDs.size() ; i++)
			{
				if (componentIDs[i] < archetypeTable.size())
				{
					for (auto& archetype : archetypeTable[componentIDs[i]])
					{
						auto& ref = archetypeMap[archetype->GetUUID()];
						ref.first = archetype;
						ref.second++;
					}
				}
			}

			int resultCounter = 0;
			for (auto& [pType, counter] : archetypeMap)
			{
				if (counter == componentIDs.size())
				{
					result[resultCounter++] = pType;
				}
			}

			return std::span<Archetype*>(result.begin(), resultCounter);
		}

		template <ValidComponent... Cs>
		std::span<Archetype*> QueryArchetype()
		{
			auto pComponentIDs = ECS::Get().allocator.MakeNew<std::array<int, sizeof...(Cs)>>();
			auto& componentIDs = *pComponentIDs;
			ExtractComponentIDs<sizeof...(Cs), Cs...>(componentIDs);

			return QueryArchetype(std::span<int>(componentIDs.begin(), componentIDs.size()));
		}

		Archetype* GetArchetype(std::span<ComponentIndex> components)
		{
			auto queriedArchetypes = QueryArchetype(components);

			for (auto i = 0 ; i < queriedArchetypes.size() ; i++)
			{
				auto pArchetype = queriedArchetypes[i];
				if (pArchetype != nullptr && pArchetype->ComponentCount() == components.size())
				{
					return pArchetype;
				}
			}

			// No existing archetype was found, adding a new one
			return AddArchetype(components);
		}

		template <ValidComponent... Cs>
		Archetype* GetArchetype()
		{
			auto pComponentIDs = ECS::Get().allocator.MakeNew<std::array<int, sizeof...(Cs)>>();
			auto& componentIDs = *pComponentIDs;
			ExtractComponentIDs<sizeof...(Cs), Cs...>(componentIDs);

			return GetArchetype(std::span<int>(componentIDs.begin(), componentIDs.size()));
		}

		static ArchetypeManager& Get()
		{
			static ArchetypeManager singleton;
			return singleton;
		}
		template <ValidComponent... Cs>
		Archetype* AddArchetype()
		{
			archetypeBuffer[archetypeCounter] = Archetype::Make<Cs...>();
			auto& newlyAddedArchetype = archetypeBuffer[archetypeCounter - 1];

			for (auto componentIndex : newlyAddedArchetype.components)
			{
				archetypeTable[componentIndex].push_back(&newlyAddedArchetype);
			}
			return &newlyAddedArchetype;
		}
		Archetype* AddArchetype(std::span<int> componentIDs)
		{
			archetypeBuffer[archetypeCounter] = Archetype::Make(componentIDs);
			auto& newlyAddedArchetype = archetypeBuffer[archetypeCounter];

			for (auto componentIndex : newlyAddedArchetype.components)
			{
				if (archetypeTable.size() < componentIndex + 1)
				{
					archetypeTable.resize(componentIndex + 1);
				}
				archetypeTable[componentIndex].push_back(&newlyAddedArchetype);
			}
			return &newlyAddedArchetype;
		}
	private:
		template <int arraySize, ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void ExtractComponentIDs(std::array<int, arraySize>& componentIDs, int index = 0)
		{
			ExtractComponentIDs<arraySize, First>(componentIDs, index++);
			ExtractComponentIDs<arraySize, Second, Rest...>(componentIDs, index);
		}
		template <int arraySize, ValidComponent C>
		void ExtractComponentIDs(std::array<int, arraySize>& componentIDs, int index = 0)
		{
			componentIDs[index] = C::UUID;
		}
		ArchetypeManager()
		{
			archetypeBuffer.resize(1000);
		}
		int ResolveUUID()
		{
			return archetypeCounter++;
		}
		int archetypeCounter = 0;
		// Indexed by componentUUID
		std::vector<std::vector<Archetype*>> archetypeTable;
		std::vector<Archetype> archetypeBuffer = {};
	};
}