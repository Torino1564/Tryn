#include "TestApp.h"
#include <Core/src/gfx/Bindables/BindableBase.h>
#include <Core/src/ent/Model/Cube.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd_, std::shared_ptr<gfx::IGraphics> gfx_)
{
	wnd = wnd_;
	gfx = gfx_;

	// Vertex Shader
	gfx::IVertexShader* pVertexShader_;
	Gfx().CreateVertexShader(L"VertexShader.cso", &pVertexShader_);
	std::unique_ptr<gfx::IVertexShader> pVertexShader(pVertexShader_);

	// Pixel Shader
	gfx::IPixelShader* pPixelShader_;
	Gfx().CreatePixelShader(L"PixelShader.cso", &pPixelShader_);
	std::unique_ptr<gfx::IPixelShader> pPixelShader(pPixelShader_);

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
	std::unique_ptr<gfx::IPolyVBuffer> pPolyVB(pPolyVB_);

	// InputLayout
	gfx::IInputLayout* pInputLayout_;
	Gfx().CreateInputLayout(*pPolyVB, *pVertexShader, &pInputLayout_);
	std::unique_ptr<gfx::IInputLayout> pPolyIL(pInputLayout_);

}

void TestApp::DoFrame()
{

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