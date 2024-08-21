#pragma once
#include "EntityID.h"
#include <Core/src/ecs/Archetype.h>
#include <Core/src/ser/StreamIO.h>

namespace tryn::ecs
{
	template <typename MapElement, ValidComponent C>
	struct SerializeWriteComponentField
	{
		void operator()(const ser::StreamWriter& streamWriter, const EntityID entityID, const bool binary = true, const std::string& name = "")
		{
			auto data = ECS::Get().archetypeManager.GetArchetype(entityID.archetype)->GetComponentData<C>();
			
			using ByteOffsetFunc_t = typename MapElement::ByteOffset_t;
			ByteOffsetFunc_t byteOffsetFunc;

			auto pData = reinterpret_cast<typename MapElement::Type*>(reinterpret_cast<std::byte*>(&data[entityID.ID - 1]) + byteOffsetFunc());

			streamWriter.Serialize(*pData, binary, name);
		}
	};

	template <typename MapElement, ValidComponent C>
	struct SerializeReadComponentField
	{
		template <typename Data = void>
		void operator()(const ser::StreamReader& streamReader, const EntityID entityID, const bool binary = true, const Data* pExtraData = nullptr)
		{
			//static_assert(HasGfxPointer<Data> && pExtraData != nullptr, "The SerializeReadComponentField functor requires extra data of type tryn::gfx::IGraphics*!");

			auto data = ECS::Get().archetypeManager.GetArchetype(entityID.archetype)->GetComponentData<C>();
			
			using ByteOffsetFunc_t = typename MapElement::ByteOffset_t;
			ByteOffsetFunc_t byteOffsetFunc;

			auto pData = reinterpret_cast<typename MapElement::Type*>(reinterpret_cast<std::byte*>(&data[entityID.ID - 1]) + byteOffsetFunc());

			streamReader.ReadSerialized(*pData, binary, pExtraData);
		}
	};
}