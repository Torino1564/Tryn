#pragma once
#include <span>
#include <vector>
#include <map>
#include <set>
#include "EcsClass.h"
#include "Core/src/ser/Serializer.h"
#include "Core/src/ser/StreamIO.h"
#include <Core/src/utl/StringHasher.h>
#include <Core/third/dynamic_bitset.hpp>

#include "Component.h"

namespace tryn::ecs
{
	using ArchetypeID = std::uint16_t;
	class Archetype
	{
		friend class Entity;
		friend class ArchetypeManager;
		template <typename T> friend struct ser::TypeSerializer;

	public:
		template <typename C>
		std::span<C> GetComponentData();

		template <typename First, typename Second, typename... Rest>
		void AppendComponents();

		template <typename C>
		void AppendComponents();

		ArchetypeID GetUUID() const;
		size_t ComponentCount() const;
		uint32_t ComponentArraySize() const;
		struct EntityID ResolveEntityUUID();
		void Free(struct EntityID);
		void Grow();
		void Resize(std::uint32_t newSize);
		ComponentArray& GetComponentArray(utl::UUID_t);
	private:
		Archetype(const ArchetypeManager& manager, const uint16_t uuid);

		template <typename... Cs>
		static Archetype Make(const ArchetypeManager& manager, const uint16_t uuid);
		static Archetype Make(const ArchetypeManager& manager, const uint16_t uuid, std::span<utl::UUID_t> componentUUIDs);

		ArchetypeID UUID = 0;
		const ComponentManager& componentManager;
		const ArchetypeManager& archetypeManager;
		std::vector<utl::UUID_t> components;

		std::uint32_t bookerPointer = 0;
		std::uint32_t upperLimit = 0;
		sul::dynamic_bitset<> booker;
		std::vector<ComponentArray> arrays;
	};

	class ArchetypeManager
	{
	public:
		friend class Archetype;

		// TODO: Access Modes

		template <typename... Cs>
		std::span<std::tuple<std::span<Cs>...>> GetComponentGroups();
		std::span<std::span<ComponentArray*>> GetComponentGroups(std::span<utl::UUID_t> componentUUIDs);

		std::span<ArchetypeID> QueryArchetype(std::span<utl::UUID_t> componentUUIDs) const;

		template <typename... Cs>
		std::span<ArchetypeID> QueryArchetype() const;

		const Archetype& GetArchetype(std::span<utl::UUID_t> components);

		template <typename... Cs>
		Archetype* GetArchetype() const;

		const Archetype& GetArchetype(const int archetypeCounter) const;

		template <typename... Cs>
		const Archetype& AddArchetype();
		const Archetype& AddArchetype(std::span<utl::UUID_t> componentUUIDs);

		ArchetypeManager(ECS* pEcs);

		[[nodiscard]]mem::ArenaAllocator<>& GetArenaAllocator() const;

	private:

		ECS* pEcs = nullptr;
		ArchetypeID ResolveUUID();
		uint16_t archetypeCounter = 0;

		std::map<utl::UUID_t, std::vector<ArchetypeID>> archetypeTable;
		std::vector<Archetype> archetypeBuffer = {};
	};
}

namespace tryn::ser
{
	template <>
	struct ser::TypeSerializer<ecs::Archetype *>
	{
		static void Write(const StreamWriter& streamWriter, ecs::Archetype* const& data, const bool binary = true, const std::string& name = "");

		static ecs::Archetype* Read(const tryn::ser::StreamReader& streamReader, const bool binary = true, const ExtraDataPack* pExtraData = nullptr);

		static void Read(ecs::Archetype*& data, const tryn::ser::StreamReader& streamReader, const bool binary = true, const ExtraDataPack* pExtraData = nullptr);
	};
}
