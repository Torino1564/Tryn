#pragma once

// check for user defined prints

#include <Core/src/gfx/ImguiManager.h>
#include <format>
#include <iostream>

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

	template <typename MapElement>
	struct ImGuiPrintElement
	{
		static void Print(typename MapElement::Type* pData)
		{
			ImGuiPrintType<typename MapElement::Type>::ImGuiPrint<MapElement>(pData);
		}
	};

}
