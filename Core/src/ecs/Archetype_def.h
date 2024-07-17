#pragma once
#include <span>
#include <vector>

#include "EcsClass.h"

namespace tryn::ecs
{
	template <ValidComponent... Cs>
	using HeterogeneusComponentDataPointerContainer = typename std::tuple<std::span<typename Cs::SubresourceData>...>;

	class Archetype
	{
		friend class Entity;
		friend class ArchetypeManager;

	public:
		template <ValidComponent... Cs>
		void FillComponentPointerTuple(std::tuple<std::span<typename Cs::SubresourceData>...>& container);

		template <int N, typename... Cs> using NthTypeOf = typename std::tuple_element<N, std::tuple<Cs...>>::type;

		template <int N = 0, ValidComponent... Cs>
		void FillComponentPointerTupleImpl(std::tuple<std::span<typename Cs::SubresourceData>...>& container);

		template <ValidComponent C>
		std::span<typename C::SubresourceData> GetComponentData();

		template <ValidComponent... Cs>
		static Archetype Make();

		static Archetype Make(std::span<int> componentIDs);

		template <ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void AppendComponents();

		template <ValidComponent C>
		void AppendComponents();

		int GetUUID() const;
		auto ComponentCount() const;
		auto ComponentArraySize() const;
		struct EntityID ResolveEntityUUID();
		void Free(struct EntityID);
		void Grow();
		void Resize(std::uint32_t newSize);

	private:
		void InitializeUUID();
		std::uint16_t UUID = 0;
		std::vector<std::uint16_t> componentSMPID;
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
		std::span<std::tuple<std::span<typename Cs::ComponentType::SubresourceData>...>> GetComponentGroups();

		std::span<Archetype*> QueryArchetype(std::span<ComponentIndex> componentIDs);

		template <ValidComponent... Cs>
		std::span<Archetype*> QueryArchetype();

		Archetype* GetArchetype(std::span<ComponentIndex> components);


		template <ValidComponent... Cs>
		Archetype* GetArchetype();

		Archetype* GetArchetype(const int archetypeCounter);

		static ArchetypeManager& Get()
		{
			static ArchetypeManager singleton;
			return singleton;
		}
		template <ValidComponent... Cs>
		Archetype* AddArchetype();
		Archetype* AddArchetype(std::span<int> componentIDs);

	private:
		template <int arraySize, ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void ExtractComponentIDs(std::array<int, arraySize>& componentIDs, int index = 0);

		template <int arraySize, ValidComponent C>
		void ExtractComponentIDs(std::array<int, arraySize>& componentIDs, int index = 0);

		ArchetypeManager();

		int ResolveUUID();
		int archetypeCounter = 0;
		// Indexed by componentUUID
		std::vector<std::vector<Archetype*>> archetypeTable;
		std::vector<Archetype> archetypeBuffer = {};
	};
}
