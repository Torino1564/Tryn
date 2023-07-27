#include "PointLight.h"
#include <Core/src/gfx/Mesh/StaticMeshPool.h>
#include "Bindables/ConstantBuffer.h"

namespace tryn::gfx
{
	PointLight::PointLight(IGraphics& gfx, float radius)
	{
		// Mesh
		mesh = StaticMeshPool::Resolve("resources/models/sphere.fbx");

		// Constant Buffer
		ConstantBufferLayout cbuflayout;
		cbuflayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float3, "position"));
		cbuflayout.Solidify();

		cbuf = IVtxConstantBuffer::Resolve(gfx, std::move(cbuflayout), 0, "position");
	}
	void PointLight::ShowControls()
	{
		if (ImGui::Begin("Point Light"))
		{
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &position.x, -60.0f, 60.0f, "%.1f");
			ImGui::SliderFloat("Y", &position.y, -60.0f, 60.0f, "%.1f");
			ImGui::SliderFloat("Z", &position.z, -60.0f, 60.0f, "%.1f");
			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}
		ImGui::End();
	}
	void PointLight::Reset()
	{
		position = { 0.0f,0.0f,0.0f };
	}
	void PointLight::Draw() const
	{

	}
}

