#pragma once
#include <Core/src/gfx/PointLightParameters.h>
#include <Core/src/ecs/cmp/PrintMemberBase.h>

namespace tryn::ecs
{
	struct PointLightComponent
	{
		gfx::PointLightParameters parameters;
	};

	template <> struct ImGuiPrintType<gfx::PointLightParameters>
	{
		template <typename MapElement>
		static void ImGuiPrint(gfx::PointLightParameters* pData)
		{
			ImGui::Text(std::format("Point light parameters:").c_str());
			ImGui::ColorEdit3("Diffuse Color", reinterpret_cast<float*>(&pData->diffuseColor));
			ImGui::ColorEdit3("Ambient", reinterpret_cast<float*>(&pData->ambient));
			ImGui::DragFloat("Constant Attenuation:", &pData->constantAtt);
			ImGui::DragFloat("Linear Attenuation:", &pData->linearAtt);
			ImGui::DragFloat("Quadratic Attenuation:", &pData->quadraticAtt);
		}
	};
}