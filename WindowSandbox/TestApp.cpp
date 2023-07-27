#include "TestApp.h"
#include <Core/src/gfx/Bindables/BindableBase.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtc/matrix_transform.hpp>
#include <Core/src/gfx/BindablePool.h>
#include <utility>
#include <Core/src/gfx/Profiler.h>
#include <Core/src/gfx/Mesh/StaticMesh.h>
#include <Core/src/gfx/Mesh/StaticMeshPool.h>
#include <Core/src/gfx/Bindables/TransformCBuf.h>
#include <Core/src/gfx/Assimp.h>

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd_, std::shared_ptr<gfx::IGraphics> gfx_)
{
	wnd = wnd_;
	gfx = gfx_;

	// Assimp Test
	const auto suzanneModel = gfx::StaticMeshPool::Resolve("resources\\models\\suzanneHp.obj");
	suzanneModel->MakeBindables(Gfx());

	// Static Object
	ent::StaticObject suzanne(suzanneModel);

	// Vertex Shader
	auto pVertexShaderFlat = gfx::IVertexShader::Resolve(Gfx(), "FlatColor_VS.cso");
	suzanne.AddBindable(pVertexShaderFlat);

	// Pixel Shader
	suzanne.AddBindable(gfx::IPixelShader::Resolve(Gfx(), "FlatColor_PS.cso"));

	// InputLayout
	suzanne.AddBindable(gfx::IInputLayout::Resolve(Gfx(), suzanne.GetVertexBuffer(), *pVertexShaderFlat));

	// Primitive Topology
	suzanne.AddBindable(gfx::IPrimitiveTopology::Resolve(Gfx()));

	// Constant Buffers
	auto pTransformCBuf = Gfx().CreateTransformCBuf();
	pTransformCBuf->BindParentMesh(suzanne);

	entities.push_back(std::move(suzanne));
}

void TestApp::DoFrame()
{
	static float angle = 0;

	static bool initialized = false;

	for (auto& entity : entities)
	{
		{
			PROFILE_SCOPE("Update Cube");
			glm::mat4 viewProjection2;
			{
				const auto eyePos = glm::vec3(0, 0, -6);
				const auto focusPoint = glm::vec3(0, 0, 0);
				const auto upDirection = glm::vec3(0, 1, 0);
				const auto view = glm::lookAtLH(eyePos, focusPoint, upDirection);
				const auto projection = glm::perspectiveFovLH(glm::radians(90.0f), (float)Gfx().dimensions.width, (float)Gfx().dimensions.height, 0.1f, 100.0f);
				viewProjection2 = projection * view;
			}
			entity.GetVtxConstantBufferByIndex(0)["transformation"]["modelViewProj"].Get<glm::mat4>() = glm::transpose(viewProjection2 *
				glm::rotate(glm::mat4(1.0f), 0.6f * angle, glm::vec3(0, 0, 1.0f)) *
				glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0, 0)) *
				glm::rotate(glm::mat4(0.5f), 2.5f * angle, glm::vec3(0, 1.0f, 0)));

			angle += 0.0001f;
		}

		{
			PROFILE_SCOPE("Draw call");
			entity.Draw(Gfx());
		}
		static bool show_demo_window = true;
		ImGui::ShowDemoWindow(&show_demo_window);
		ImGui::Begin("Test", &show_demo_window, 0);
		ImGui::End();
	}
}