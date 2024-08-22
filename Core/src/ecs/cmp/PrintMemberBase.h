#pragma once
#include <Core/src/gfx/ImguiManager.h>
#include <format>
#include <Core/src/ecs/EntityID.h>
#include <Core/src/ecs/EcsClass.h>
#include <Core/src/ecs/Archetype_def.h>

namespace tryn::ecs::cmp
{
	template <typename ElementType>
	struct ImGuiPrintType
	{
		template <typename MapElement>
		static void ImGuiPrint(ElementType* pData)
		{
			using VarNameFunc_t = typename MapElement::VarName_t;
			using TypeNameFunc_t = typename MapElement::TypeName_t;

			VarNameFunc_t varNameFunc;
			TypeNameFunc_t typeNameFunc;

			ImGui::Text(std::format("{}: {}",typeNameFunc(), varNameFunc()).c_str());
		}
	};

	template <typename MapElement, ValidComponent C>
	struct PrintImGuiMemberVariable
	{
		void operator()(EntityID entityUUID)
		{
			auto pArchetype = ECS::Get().archetypeManager.GetArchetype(entityUUID.archetype);
			auto data = pArchetype->GetComponentData<C>();

			using ByteOffsetFunc_t = typename MapElement::ByteOffset_t;
			ByteOffsetFunc_t byteOffsetFunc;

			auto pData = reinterpret_cast<typename MapElement::Type*>(reinterpret_cast<std::byte*>(&data[entityUUID.ID - 1]) + byteOffsetFunc());

			ImGuiPrintType<typename MapElement::Type>::ImGuiPrint<MapElement>(pData);
		}
	};
}
