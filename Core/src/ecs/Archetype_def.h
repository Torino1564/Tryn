#pragma once
#include <span>
#include <vector>
#include <map>
#include <set>
#include "Core/src/ser/Serializer.h"
#include "Core/src/ser/StreamIO.h"
#include <Core/src/utl/StringHasher.h>
#include <Core/third/dynamic_bitset.hpp>

#include "Component.h"

namespace tryn::ecs
{
	class Archetype;
}

namespace tryn::ser
{
	void Serialize(StreamIO& io, ecs::Archetype* data, bool binary = true, const std::string& name = "");
}

namespace tryn::ecs
{
	class ECS;
	using ArchetypeID = std::uint32_t;
	class Archetype
	{
		friend class Entity;
		friend class ArchetypeManager;
		friend void ser::Serialize(ser::StreamIO& io, Archetype* data, bool binary, const std::string& name);

	public:
		template <typename C>
		std::span<C> GetComponentData();

		template <typename First, typename Second, typename... Rest>
		void AppendComponents();

		template <typename C>
		void AppendComponents();
		bool HasEntity(EntityID id) const;
		ArchetypeID GetUUID() const;
		size_t ComponentCount() const;
		uint32_t ComponentArraySize() const;
		EntityID ResolveEntityUUID();
		void Free(EntityID);
		void Grow();
		void Resize(std::uint32_t newSize);
		ComponentArray& GetComponentArray(utl::UUID_t);

		void EntityControlWindow(EntityID id);
		const ArchetypeManager& Manager() const;
	private:
		EntityID ResolveEntityUUID_Impl(bool defaultInit);
		Archetype(ArchetypeManager& manager, uint16_t uuid);

		template <typename... Cs>
		static Archetype Make(ArchetypeManager& manager, const uint16_t uuid);
		static Archetype Make(ArchetypeManager& manager, const uint16_t uuid, std::span<utl::UUID_t> componentUUIDs);

		ArchetypeID UUID = 0;
		ComponentManager& componentManager;
		ArchetypeManager& archetypeManager;
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

		template <typename... ACs>
		std::span<std::tuple<std::span<typename ACs::Component>...>> GetComponentGroups();

		template <typename... ACs>
		std::pair<std::span<ArchetypeID>, std::span<std::tuple<std::span<typename ACs::Component>...>>> GetComponentGroupsEx();

		std::span<std::span<ComponentArray*>> GetComponentGroups(std::span<utl::UUID_t> componentUUIDs);

		std::span<ArchetypeID> QueryArchetype(std::span<utl::UUID_t> componentUUIDs);

		template <typename... Cs>
		std::span<ArchetypeID> QueryArchetype();

		template <typename... Cs>
		Archetype& GetArchetype();

		Archetype& GetArchetype(int archetypeCounter);
		Archetype& GetArchetype(std::span<utl::UUID_t> componentUUIDs);

		template <typename... Cs>
		Archetype& AddArchetype();
		Archetype& AddArchetype(std::span<utl::UUID_t> componentUUIDs);

		EntityID MoveEntity(Archetype& destination, EntityID entityID);

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
