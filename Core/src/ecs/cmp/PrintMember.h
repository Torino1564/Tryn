#pragma once

#if __has_include(<EngineConfigs/ImGuiPrintTypes.h>)
	#pragma message("Using user ImGui Print Types")
	#include <EngineConfigs/ImGuiPrintTypes.h>
#else
	#include "PrintMemberBase.h"
#endif

#include "Core/third/glm/vec3.hpp"

namespace tryn::ecs::cmp
{
	template <> struct ImGuiPrintType<glm::vec3>
	{
		template <typename MapElement>
		static void ImGuiPrint(glm::vec3* pData)
		{
			using VarNameFunc_t = typename MapElement::VarName_t;
			using TypeNameFunc_t = typename MapElement::TypeName_t;

			VarNameFunc_t varNameFunc;
			TypeNameFunc_t typeNameFunc;

			ImGui::SliderFloat3(std::format("{}: {}", typeNameFunc(), varNameFunc()).c_str(), (float*)pData, -100.0f, 100.0f);
		}
	};
}