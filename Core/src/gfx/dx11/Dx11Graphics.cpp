#include "Dx11Graphics.h"
#include "GraphicsError.h"
#include <iostream>
#include <d3dcompiler.h>
#include <fstream>
#include <Core/src/gfx/dx11/Bindables/DX11VertexBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11InputLayout.h>
#include <Core/src/gfx/dx11/Bindables/DX11VertexShader.h>
#include <core/src/gfx/dx11/Bindables/DX11IndexBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11PixelShader.h>
#include <Core/src/gfx/dx11/Bindables/DX11ConstantBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11PrimitiveTopology.h>
#include <core/src/ent/Model/Model.h>
#include <Core/src/ent/Model/Cube.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtc/matrix_transform.hpp>


#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"D3DCompiler.lib")

namespace tryn::gfx::dx11
{
	Graphics::Graphics(HWND hWnd, int width, int height)
	{
		DXGI_SWAP_CHAIN_DESC swapDesc = {};
		swapDesc.BufferDesc.Width = width;
		swapDesc.BufferDesc.Height = height;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapDesc.BufferDesc.RefreshRate.Denominator = 0;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.BufferCount = 1;
		swapDesc.OutputWindow = hWnd;
		swapDesc.Windowed = TRUE;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.Flags = 0;

		UINT swapCreateFlags = 0u;
#ifndef NDEBUG
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			swapCreateFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapDesc,
			&pSwap,
			&pDevice,
			nullptr,
			&pContext
		) >> chk;

		// gain access to texture subresource in swap chain (back buffer)
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
		pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer) >> chk;
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pTarget.ReleaseAndGetAddressOf());

		dimensions.height = height;
		dimensions.width = width;

		// viewport always fullscreen (for now)
		D3D11_VIEWPORT vp;
		vp.Width = (float)width;
		vp.Height = (float)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		pContext->RSSetViewports(1u, &vp);
	}

	Graphics::~Graphics()
	{

	}

	void Graphics::BeginFrame()
	{
	}

	void Graphics::EndFrame()
	{
		pSwap->Present(1u, 0u) >> chk;
	}

	void Graphics::ClearBuffer(float r, float g, float b)
	{
		const float color[]{ r, g, b, 1.0f };

		pContext->ClearRenderTargetView(pTarget.Get(), color);
	}

	void Graphics::DrawTriangle()
	{
		std::shared_ptr<VertexBuffer> cpuBuffer = std::make_shared<VertexBuffer>(std::move(VertexLayout(VertexLayout::Position3D, VertexLayout::Char4Color)), 8);

		DX11VertexBuffer vertexBuffer(*this, cpuBuffer);

		vertexBuffer[0](glm::vec3{-1.0f, -1.0f, -1.0f	}, BGRAColor{ 255,0,0 });
		vertexBuffer[1](glm::vec3{1.0f, -1.0f, -1.0f	}, BGRAColor{ 0,255,0 });
		vertexBuffer[2](glm::vec3{-1.0f, 1.0f, -1.0f	}, BGRAColor{ 0,0,255 });
		vertexBuffer[3](glm::vec3{1.0f, 1.0f, -1.0f		}, BGRAColor{ 255,255,0 });
		vertexBuffer[4](glm::vec3{-1.0f, -1.0f, 1.0f	}, BGRAColor{ 0,255,255 });
		vertexBuffer[5](glm::vec3{1.0f, -1.0f, 1.0f		}, BGRAColor{ 255,0,255 });
		vertexBuffer[6](glm::vec3{-1.0f, 1.0f, 1.0f		}, BGRAColor{ 255,255,255 });
		vertexBuffer[7](glm::vec3{1.0f, 1.0f, 1.0f		}, BGRAColor{ 0,255,0 });

		vertexBuffer.Bind();

		namespace wrl = Microsoft::WRL;

		// Index Buffer
		
		ent::Model& Model = ent::Cube::GetInstance();

		Model.MakeBindables(*this);

		Model.GetBindables().back()->Bind();

		wrl::ComPtr<ID3DBlob> pBlob;

		// Pixel shader

		DX11PixelShader pixelShader(*this, L"PixelShader.cso");
		pixelShader.Bind();


		// Vertex shader

		DX11VertexShader vertexShader(*this, L"VertexShader.cso");
		vertexShader.Bind();

		// input layout

		DX11InputLayout inputLayout(*this, vertexBuffer, vertexShader);
		inputLayout.Bind();

		// Cbuffer
		static float angle = 0;

		ConstantBufferLayout cblayout;
		cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "transformation"));
		cblayout.Solidify();
		DX11ConstantBuffer cbuf(*this, std::move(cblayout));

		glm::mat4 viewProjection2;
		{
			const auto eyePos = glm::vec3(0, 0, -6);
			const auto focusPoint = glm::vec3(0, 0, 0);
			const auto upDirection = glm::vec3(0, 1, 0);
			const auto view = glm::lookAtLH(eyePos, focusPoint, upDirection);
			const auto projection = glm::perspectiveFovLH(glm::radians(90.0f), (float)dimensions.width, (float)dimensions.height, 0.1f, 100.0f);
			viewProjection2 = projection * view;
		}

		cbuf["transformation"].Get<glm::mat4>() = glm::transpose(viewProjection2 *
			glm::rotate(glm::mat4(1.0f), 0.6f * angle, glm::vec3(0, 0, 1.0f)) *
			glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0, 0)) *
			glm::rotate(glm::mat4(0.5f), 2.5f * angle, glm::vec3(0, 1.0f, 0))
		);

		cbuf.Bind();

		angle += 0.01;

		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

		// Primitive Topology
		DX11PrimitiveTopology topology(*this);
		topology.Bind();

		pContext->DrawIndexed( (UINT)36, 0u, 0u);
	}
	void Graphics::DrawIndexed(int count)
	{
		pContext->DrawIndexed(count, 0u, 0u);
	}
	GraphicAPI Graphics::GetType()	
	{
		return GraphicAPI::DX11;
	}
	void Graphics::MakeBindablesForModel(ent::Model& model)
	{
		auto& bindables = model.GetBindables();

		bindables.push_back(std::make_unique<DX11VertexBuffer>(*this, model.GetBuffer()));
		bindables.push_back(std::make_unique<DX11IndexBuffer>(*this, model.GetIndices()));
	}
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& Graphics::GetContext()
	{
		return pContext;
	}
	Microsoft::WRL::ComPtr<ID3D11Device>& Graphics::GetDevice()
	{
		return pDevice;
	}


}