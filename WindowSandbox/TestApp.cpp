#include "TestApp.h"
#include <Core/src/gfx/Bindables/BindableBase.h>
#include <Core/src/ent/Model/Cube.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtc/matrix_transform.hpp>
#include <Core/src/gfx/BindablePool.h>
#include <ranges>

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd_, std::shared_ptr<gfx::IGraphics> gfx_)
{
	wnd = wnd_;
	gfx = gfx_;

	ent::Entity testEntity;

	std::vector<int> indices = {
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
	};

	ent::Model testModel({},  indices,"cube");
	testEntity.model = std::make_shared<ent::Model>(testModel);

	// Vertex Shader
	std::string pathVS = "VertexShader.cso";
	auto pVertexShader = gfx::BindablePool::Resolve<gfx::IVertexShader>(Gfx(),pathVS);
	testEntity.bindables.push_back(pVertexShader);

	// Pixel Shader
	testEntity.bindables.push_back(Gfx().CreatePixelShader("PixelShader.cso"));

	// Index Buffer
	testEntity.bindables.push_back(Gfx().CreateIndexBuffer("cubee",std::make_shared<std::vector<int>>(indices)));
	auto dummyIB = gfx::BindablePool::Resolve<gfx::IIndexBuffer>(Gfx(), "cube", std::make_shared<std::vector<int>>(indices));

	// PolyVertexBuffer
	auto posCPUBuffer = ent::Cube::GetVertexBuffer();

	std::shared_ptr<gfx::VertexBuffer> colorCPUBuffer = std::make_shared<gfx::VertexBuffer>(gfx::VertexLayout(gfx::VertexLayout::VertexElement::Char4Color), posCPUBuffer->Size());
	(*colorCPUBuffer)[0].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 255,0,0 };
	(*colorCPUBuffer)[1].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 0,255,0 };
	(*colorCPUBuffer)[2].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 0,0,255 };
	(*colorCPUBuffer)[3].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 255,255,0 };
	(*colorCPUBuffer)[4].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 0,255,255 };
	(*colorCPUBuffer)[5].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 255,0,255 };
	(*colorCPUBuffer)[6].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 255,255,255 };
	(*colorCPUBuffer)[7].Attr<gfx::VertexLayout::VertexElement::Char4Color>() = BGRAColor{ 0,255,0 };

	std::vector<std::shared_ptr<gfx::VertexBuffer>> vertexBufferArr =
	{
		posCPUBuffer,
		colorCPUBuffer
	};
	auto pPolyVB = Gfx().CreatePolyVertexBuffer(vertexBufferArr);
	testEntity.bindables.push_back(pPolyVB);

	auto dummyVB5 = gfx::BindablePool::Resolve<gfx::IVertexBuffer>(Gfx(),posCPUBuffer);

	// InputLayout
	testEntity.bindables.push_back(Gfx().CreateInputLayout(*pPolyVB, *pVertexShader));

	// Primitive Topology
	testEntity.bindables.push_back(Gfx().CreatePrimitiveTopology());

	// Constant Buffer
	gfx::ConstantBufferLayout cBufLayout;
	cBufLayout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Matrix4, "transformation"));
	cBufLayout.Solidify();

	testEntity.pConstantBuffer = Gfx().CreateConstantBuffer(std::move(cBufLayout));

	
	entities.push_back(std::move(testEntity));
}

void TestApp::DoFrame()
{
	static float angle = 0;
	

	for (auto& entity : entities)
	{
		glm::mat4 viewProjection2;
		{
			const auto eyePos = glm::vec3(0, 0, -6);
			const auto focusPoint = glm::vec3(0, 0, 0);
			const auto upDirection = glm::vec3(0, 1, 0);
			const auto view = glm::lookAtLH(eyePos, focusPoint, upDirection);
			const auto projection = glm::perspectiveFovLH(glm::radians(90.0f), (float)Gfx().dimensions.width, (float)Gfx().dimensions.height, 0.1f, 100.0f);
			viewProjection2 = projection * view;
		}
		(*entity.pConstantBuffer)["transformation"].Get<glm::mat4>() = glm::transpose(viewProjection2 *
			glm::rotate(glm::mat4(1.0f), 0.6f * angle, glm::vec3(0, 0, 1.0f)) *
			glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0, 0)) *
			glm::rotate(glm::mat4(0.5f), 2.5f * angle, glm::vec3(0, 1.0f, 0)));

		entity.Draw(Gfx());
	}
	angle += 0.01;
}