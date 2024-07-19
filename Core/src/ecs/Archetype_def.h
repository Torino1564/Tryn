#pragma once
#include <span>
#include <vector>

#include "EcsClass.h"
#include "Core/src/ser/Serializer.h"
#include "Core/src/ser/StreamIO.h"

namespace tryn::ecs
{
	template <ValidComponent... Cs>
	using HeterogeneusComponentDataPointerContainer = typename std::tuple<std::span<typename Cs::SubresourceData>...>;

	class Archetype
	{
		friend class Entity;
		friend class ArchetypeManager;
		template <typename T> friend struct ser::TypeSerializer;

	public:
		template <ValidComponent... Cs>
		void FillComponentPointerTuple(std::tuple<std::span<typename Cs::SubresourceData>...>& container);

		template <int N, typename... Cs> using NthTypeOf = typename std::tuple_element<N, std::tuple<Cs...>>::type;

		template <int N = 0, ValidComponent... Cs>
		void FillComponentPointerTupleImpl(std::tuple<std::span<typename Cs::SubresourceData>...>& container);

		template <ValidComponent C, bool Index = true>
		std::span<typename C::SubresourceData> GetComponentData();

		template <ValidComponent... Cs>
		static Archetype Make();

		static Archetype Make(std::span<utl::UUID_t> componentIDs);

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
		template <auto Tag = []{}>
		void InitSortedComponentUUIDs();

		void InitializeUUID();
		std::uint16_t UUID = 0;
		std::vector<utl::UUID_t> componentUUIDs;
		std::vector<unsigned int> components;

		std::vector<utl::UUID_t> sortedComponentUUIDs;

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

		std::span<Archetype*> QueryArchetype(std::span<utl::UUID_t> componentIDs) const;

		template <ValidComponent... Cs>
		std::span<Archetype*> QueryArchetype();

		Archetype* GetArchetype(std::span<utl::UUID_t> components);


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
		Archetype* AddArchetype(std::span<utl::UUID_t> componentIDs);

	private:
		enum ComponentIdentifier
		{
			Index,
			UUID
		};
		template <int arraySize, ComponentIdentifier Type, ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void ExtractComponentIDs(std::array<utl::UUID_t, arraySize>& componentIDs, int index = 0);

		template <int arraySize, ComponentIdentifier Type, ValidComponent C>
		void ExtractComponentIDs(std::array<utl::UUID_t, arraySize>& componentIDs, int index = 0);

		ArchetypeManager();

		int ResolveUUID();
		int archetypeCounter = 0;
		// Indexed by componentUUID
		std::vector<std::vector<Archetype*>> archetypeTable;
		std::vector<Archetype> archetypeBuffer = {};
	};
}

namespace tryn::ser
{
	template <>
	struct ser::TypeSerializer<ecs::Archetype*>
	{
		static void Write(const ser::StreamWriter& streamWriter, const ecs::Archetype*& data, const bool binary = true, const std::string& name = "")
		{
			std::vector<utl::UUID_t> sortedComponents = data->componentUUIDs;
			std::ranges::sort(sortedComponents);
			streamWriter.Serialize(sortedComponents, binary, name);
		}

		static ecs::Archetype* Read(const tryn::ser::StreamReader& streamReader, const bool binary = true)
		{
			std::vector<utl::UUID_t> componentUUIDs;

			streamReader.ReadSerialized(componentUUIDs, binary);

			return ecs::ECS::Get().archetypeManager.GetArchetype(componentUUIDs);
		}

		static void Read(ecs::Archetype*& data, const tryn::ser::StreamReader& streamReader, const bool binary = true)
		{
			std::vector<utl::UUID_t> componentUUIDs;
			streamReader.ReadSerialized(componentUUIDs, binary);
			data = ecs::ECS::Get().archetypeManager.GetArchetype(componentUUIDs);
		}
	};
}
