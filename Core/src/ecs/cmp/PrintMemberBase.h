#pragma once
#include <Core/src/gfx/ImguiManager.h>
#include <format>
#include <Core/src/ecs/EntityID.h>
#include <Core/src/ecs/EcsClass.h>
#include <Core/src/ecs/Archetype_def.h>
#include <typeinfo>

namespace tryn::ecs
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

	template <typename T>
	class ImGuiPrintManager
	{
		static constexpr auto& uuid = typeid(T);
	};

}
