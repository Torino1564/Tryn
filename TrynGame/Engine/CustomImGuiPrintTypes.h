#pragma once
#include <Core/src/ecs/cmp/PrintMemberBase.h>

namespace tryn::ecs::cmp
{
	template <> struct ImGuiPrintType<float>
	{
		template <typename MapElement>
		static void ImGuiPrint(float* pData)
		{
			using VarNameFunc_t = typename MapElement::VarName_t;
			using TypeNameFunc_t = typename MapElement::TypeName_t;

			VarNameFunc_t varNameFunc;
			TypeNameFunc_t typeNameFunc;

			ImGui::DragFloat(std::format("{}: {}", typeNameFunc(), varNameFunc()).c_str(), pData);
		}
	};

	template <> struct ImGuiPrintType<bool>
	{
		template <typename MapElement>
		static void ImGuiPrint(bool* pData)
		{
			using VarNameFunc_t = typename MapElement::VarName_t;
			using TypeNameFunc_t = typename MapElement::TypeName_t;

			VarNameFunc_t varNameFunc;
			TypeNameFunc_t typeNameFunc;

			ImGui::Checkbox(std::format("{}: {}", typeNameFunc(), varNameFunc()).c_str(), pData);
		}
	};
}