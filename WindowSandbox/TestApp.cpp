#include "TestApp.h"
#include <Core/src/gfx/Bindables/BindableBase.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtc/matrix_transform.hpp>
#include <utility>
#include <Core/src/gfx/Profiler.h>
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <core/src/gfx/RenderQueue/Technique.h>
#include <Core/src/gfx/RenderQueue/Step.h>
#include <Core/src/gfx/Assimp.h>

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd, std::shared_ptr<gfx::IGraphics> gfx)
{
	this->wnd = std::move(wnd);
	this->gfx = std::move(gfx);

	// Graphic Matrices
	constexpr auto eyePos = glm::vec3(0, 0, -6);
	constexpr auto focusPoint = glm::vec3(0, 0, 0);
	constexpr auto upDirection = glm::vec3(0, 1, 0);
	Gfx().SetCamera(glm::lookAtLH(eyePos, focusPoint, upDirection));
	Gfx().SetProjection(glm::perspectiveFovLH(glm::radians(90.0f), static_cast<float>(Gfx().dimensions.width), static_cast<float>(Gfx().dimensions.height), 0.1f, 100.0f));

	// Suzanne
	{
		ent::StaticObject suzanne(Gfx(), "resources\\models\\suzanneHp.obj");

		gfx::Technique flat("Phong");
		{
			gfx::Step first;
			{
				// Pixel Shader
				first.AddBindable(gfx::IPixelShader::Resolve(Gfx(), "PhongFlatColor_PS.cso"));

				// Vertex Shader
				auto pVs = gfx::IVertexShader::Resolve(Gfx(), "PhongFlatColor_VS.cso");

				// InputLayout
				first.AddBindable(gfx::IInputLayout::Resolve(Gfx(), suzanne.GetVertexBuffer(), *pVs));
				first.AddBindable(std::move(pVs));
				
				// Px CBuf
				{
					// Point Light
					{
						auto layout = gfx::ConstantBufferLayout();
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float3, "viewLightPos"));
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float3, "ambient"));
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float3, "diffuse color"));
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float, "diffuse intensity"));
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float, "constant att"));
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float, "linear att"));
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float, "quadratic att"));
						layout.Solidify();
						first.AddBindable(gfx::IPxConstantBuffer::Resolve(Gfx(), std::move(layout), 0, "pointLight"));
					}
					// Object Cbuf
					{
						auto layout = gfx::ConstantBufferLayout();
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float3, "material Color"));
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float3, "specular Color"));
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float, "specular weight"));
						layout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float, "specular gloss"));
						layout.Solidify();
						first.AddBindable(gfx::IPxConstantBuffer::Resolve(Gfx(), std::move(layout), 1, "object cbuf"));
					}

				}

			}
			flat.AddStep(std::move(first));
		}
		suzanne.AddTechnique(std::move(flat));

		entities.push_back(std::move(suzanne));
	}

	for (auto& entity : entities)
	{
		entity.BindParent();
	}
}

void TestApp::DoFrame()
{
	for (auto& entity : entities)
	{
		{
			static constexpr float angle = 0.001f;
			PROFILE_SCOPE("Update Rotation");
			entity.GetYaw() += 0.3f * angle;
			entity.GetPitch() += 0.1f * angle;
			entity.GetRoll() += 0.7f * angle;
		}
		{
			PROFILE_SCOPE("Draw call");
			entity.Draw(Gfx());
		}
		static bool showDemoWindow = true;
		ImGui::ShowDemoWindow(&showDemoWindow);
		ImGui::Begin("Test", &showDemoWindow, 0);
		ImGui::End();
	}
}