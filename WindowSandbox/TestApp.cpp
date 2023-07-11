#include "TestApp.h"
#include <Core/src/gfx/Bindables/BindableBase.h>
#include <Core/src/ent/Model/Cube.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtc/matrix_transform.hpp>

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

	ent::Model testModel({},  indices);
	testEntity.model = std::make_shared<ent::Model>(testModel);

	// Vertex Shader
	gfx::IVertexShader* pVertexShader_;
	Gfx().CreateVertexShader(L"VertexShader.cso", &pVertexShader_);
	std::shared_ptr<gfx::IVertexShader> pVertexShader(pVertexShader_);
	testEntity.bindables.push_back(pVertexShader);

	// Pixel Shader
	gfx::IPixelShader* pPixelShader_;
	Gfx().CreatePixelShader(L"PixelShader.cso", &pPixelShader_);
	std::shared_ptr<gfx::IPixelShader> pPixelShader(pPixelShader_);
	testEntity.bindables.push_back(pPixelShader);

	// Index Buffer
	gfx::IIndexBuffer* pIndexBuffer_;
	Gfx().CreateIndexBuffer(std::make_shared<std::vector<int>>(indices), &pIndexBuffer_);
	std::shared_ptr<gfx::IIndexBuffer> pIndexBuffer(pIndexBuffer_);
	testEntity.bindables.push_back(pIndexBuffer);

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

	gfx::IPolyVBuffer* pPolyVB_;
	std::vector<std::shared_ptr<gfx::VertexBuffer>> vertexBufferArr =
	{
		posCPUBuffer,
		colorCPUBuffer
	};
	Gfx().CreatePolyVertexBuffer(vertexBufferArr , &pPolyVB_);
	std::shared_ptr<gfx::IPolyVBuffer> pPolyVB(pPolyVB_);
	testEntity.bindables.push_back(pPolyVB);

	// InputLayout
	gfx::IInputLayout* pInputLayout_;
	Gfx().CreateInputLayout(*pPolyVB, *pVertexShader, &pInputLayout_);
	std::shared_ptr<gfx::IInputLayout> pInputLayout(pInputLayout_);
	testEntity.bindables.push_back(pInputLayout);

	// Primitive Topology
	gfx::IPrimitiveTopology* pPTopology_;
	Gfx().CreatePrimitiveTopology(&pPTopology_);
	std::shared_ptr<gfx::IPrimitiveTopology> pPrimitiveTopology(pPTopology_);
	testEntity.bindables.push_back(pPrimitiveTopology);

	// Constant Buffer
	gfx::ConstantBufferLayout cBufLayout;
	cBufLayout.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Matrix4, "transformation"));
	cBufLayout.Solidify();

	gfx::IConstantBuffer* pCBuf_;
	Gfx().CreateConstantBuffer(std::move(cBufLayout), &pCBuf_);
	std::shared_ptr<gfx::IConstantBuffer> pConstantBuffer(pCBuf_);
	testEntity.pConstantBuffer = pConstantBuffer;

	
	
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
 
//std::shared_ptr<VertexBuffer> cpuBuffer = std::make_shared<VertexBuffer>(std::move(VertexLayout(VertexLayout::Position3D, VertexLayout::Char4Color)), 8);
//
//DX11VertexBuffer vertexBuffer(*this, cpuBuffer);
//
//vertexBuffer[0](glm::vec3{-1.0f, -1.0f, -1.0f	}, BGRAColor{ 255,0,0 });
//vertexBuffer[1](glm::vec3{1.0f, -1.0f, -1.0f	}, BGRAColor{ 0,255,0 });
//vertexBuffer[2](glm::vec3{-1.0f, 1.0f, -1.0f	}, BGRAColor{ 0,0,255 });
//vertexBuffer[3](glm::vec3{1.0f, 1.0f, -1.0f	}, BGRAColor{ 255,255,0 });
//vertexBuffer[4](glm::vec3{-1.0f, -1.0f, 1.0f	}, BGRAColor{ 0,255,255 });
//vertexBuffer[5](glm::vec3{1.0f, -1.0f, 1.0f	}, BGRAColor{ 255,0,255 });
//vertexBuffer[6](glm::vec3{-1.0f, 1.0f, 1.0f	}, BGRAColor{ 255,255,255 });
//vertexBuffer[7](glm::vec3{1.0f, 1.0f, 1.0f	}, BGRAColor{ 0,255,0 });
//
//vertexBuffer.Bind();
//
//namespace wrl = Microsoft::WRL;
//
//// Index Buffer
//
//ent::Model Model = ent::Cube::GetInstance();
//
//Model.MakeBindables(*this);
//
//Model.GetBindables().back()->Bind();
//
//wrl::ComPtr<ID3DBlob> pBlob;
//
//// Pixel shader
//
//DX11PixelShader pixelShader(*this, L"PixelShader.cso");
//pixelShader.Bind();
//
//
//// Vertex shader
//
//DX11VertexShader vertexShader(*this, L"VertexShader.cso");
//vertexShader.Bind();
//
//// input layout
//
//DX11InputLayout inputLayout(*this, vertexBuffer, vertexShader);
//inputLayout.Bind();
//
//// Cbuffer
//static float angle = 0;
//
//ConstantBufferLayout cblayout;
//cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "transformation"));
//cblayout.Solidify();
//DX11ConstantBuffer cbuf(*this, std::move(cblayout));
//
//glm::mat4 viewProjection2;
//{
//	const auto eyePos = glm::vec3(0, 0, -6);
//	const auto focusPoint = glm::vec3(0, 0, 0);
//	const auto upDirection = glm::vec3(0, 1, 0);
//	const auto view = glm::lookAtLH(eyePos, focusPoint, upDirection);
//	const auto projection = glm::perspectiveFovLH(glm::radians(90.0f), (float)dimensions.width, (float)dimensions.height, 0.1f, 100.0f);
//	viewProjection2 = projection * view;
//}
//
//cbuf["transformation"].Get<glm::mat4>() = glm::transpose(viewProjection2 *
//	glm::rotate(glm::mat4(1.0f), 0.6f * angle, glm::vec3(0, 0, 1.0f)) *
//	glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0, 0)) *
//	glm::rotate(glm::mat4(0.5f), 2.5f * angle, glm::vec3(0, 1.0f, 0))
//);
//
//cbuf.Bind();
//
//angle += 0.01;
//
//pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);
//
//// Primitive Topology
//DX11PrimitiveTopology topology(*this);
//topology.Bind();
//
//pContext->DrawIndexed((UINT)36, 0u, 0u);