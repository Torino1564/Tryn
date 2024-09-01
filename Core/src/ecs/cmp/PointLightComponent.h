#pragma once
#include "ComponentManager.h"
#include <Core/src/gfx/PointLightParameters.h>
#include <Core/src/ecs/cmp/PrintMemberBase.h>

namespace tryn::ecs
{
	ZT_DEFINE_COMPONENT(PointLightComponent)
	{
		public: struct SubresourceData{ gfx::PointLightParameters parameters; using parameters_t = tryn::utl::CTM::Map_t<gfx::PointLightParameters, "gfx::PointLightParameters", "parameters", sizeof(gfx::PointLightParameters), UUID>; }; const static inline SubresourceData srd = {}; static const unsigned int index;
		static const std::vector<utl::CTM::ElementData>& GetReflectData_();
		static const std::vector<utl::CTM::ElementData>& vec;
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