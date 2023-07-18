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

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd_, std::shared_ptr<gfx::IGraphics> gfx_)
{
	wnd = wnd_;
	gfx = gfx_;

	// Mesh creation
	
	std::vector<int> indices = {
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
	};

	gfx::VertexBuffer posCPUBuffer(gfx::VertexLayout(gfx::VertexLayout::VertexElement::Position3D), 8);
	posCPUBuffer[0].Attr<gfx::VertexLayout::VertexElement::Position3D>() = { -1.0f,-1.0f,-1.0f };
	posCPUBuffer[1].Attr<gfx::VertexLayout::VertexElement::Position3D>() = { 1.0f,-1.0f,-1.0f };
	posCPUBuffer[2].Attr<gfx::VertexLayout::VertexElement::Position3D>() = { -1.0f,1.0f,-1.0f };
	posCPUBuffer[3].Attr<gfx::VertexLayout::VertexElement::Position3D>() = { 1.0f,1.0f,-1.0f };
	posCPUBuffer[4].Attr<gfx::VertexLayout::VertexElement::Position3D>() = { -1.0f,-1.0f,1.0f };
	posCPUBuffer[5].Attr<gfx::VertexLayout::VertexElement::Position3D>() = { 1.0f,-1.0f,1.0f };
	posCPUBuffer[6].Attr<gfx::VertexLayout::VertexElement::Position3D>() = { -1.0f,1.0f,1.0f };
	posCPUBuffer[7].Attr<gfx::VertexLayout::VertexElement::Position3D>() = { 1.0f,1.0f,1.0f };

	std::vector bfarray = { posCPUBuffer };

	gfx::StaticMesh cubeMesh(std::move(bfarray), std::move(indices), "cube");
	cubeMesh.MakeBindables(Gfx());

	// Static Object
	ent::StaticObject cube1(std::make_shared<gfx::StaticMesh>(cubeMesh));
	
	// Vertex Buffer
	gfx::VertexBuffer colorCPUBuffer(gfx::VertexLayout(gfx::VertexLayout::VertexElement::Char4Color), 8);
	colorCPUBuffer[0].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 255,0,0 };
	colorCPUBuffer[1].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 0,255,0 };
	colorCPUBuffer[2].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 0,0,255 };
	colorCPUBuffer[3].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 255,255,0 };
	colorCPUBuffer[4].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 0,255,255 };
	colorCPUBuffer[5].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 255,0,255 };
	colorCPUBuffer[6].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 255,255,255 };
	colorCPUBuffer[7].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 0,255,0 };

	{
		gfx::BufferArray extraData;
		extraData.push_back(std::move(std::pair{ "coloredCube",std::make_shared<gfx::VertexBuffer>(colorCPUBuffer) }));
		cube1.ExpandMeshVertexBuffer(Gfx(),extraData);
	}

	// Vertex Shader
	std::string pathVS = "VertexShader.cso";
	auto pVertexShader = gfx::IVertexShader::Resolve(Gfx(), pathVS);
	cube1.AddBindable(pVertexShader);

	// Pixel Shader
	auto pPixelShader = gfx::IPixelShader::Resolve(Gfx(), "PixelShader.cso");
	cube1.AddBindable(pPixelShader);

	// InputLayout
	auto pInputLayout = gfx::IInputLayout::Resolve(Gfx(), cube1.GetVertexBuffer(), *pVertexShader);
	cube1.AddBindable(pInputLayout);

	// Primitive Topology
	auto pPrimitiveTopology = gfx::IPrimitiveTopology::Resolve(Gfx());
	cube1.AddBindable(pPrimitiveTopology);

	// Constant Buffer
	gfx::ConstantBufferLayout cBufLayout;
	cBufLayout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Matrix4, "transformation"));
	cBufLayout.Solidify();
	auto pConstantBuffer = gfx::IConstantBuffer::Resolve(Gfx(), std::move(cBufLayout), 0, "cubeTransformation");
	cube1.SetConstantBuffer(pConstantBuffer);

	entities.push_back(std::move(cube1));
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
			entity.GetConstantBuffer()["transformation"].Get<glm::mat4>() = glm::transpose(viewProjection2 *
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