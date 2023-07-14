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
#include <Core/src/gfx/dx11/Bindables/DX11PolyVBuffer.h>
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
		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);
		ClearBuffer();
	}

	void Graphics::EndFrame()
	{
		pSwap->Present(0u, 0u) >> chk;
	}

	void Graphics::ClearBuffer(float r, float g, float b)
	{
		const float color[]{ r, g, b, 1.0f };

		pContext->ClearRenderTargetView(pTarget.Get(), color);
	}

	void Graphics::DrawTriangle()
	{

		pContext->DrawIndexed((UINT)36, 0u, 0u);
	}
	void Graphics::DrawIndexed(int count)
	{
		pContext->DrawIndexed(count, 0u, 0u);
	}
	GraphicAPI Graphics::GetType()
	{
		return GraphicAPI::DX11;
	}
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& Graphics::GetContext()
	{
		return pContext;
	}
	Microsoft::WRL::ComPtr<ID3D11Device>& Graphics::GetDevice()
	{
		return pDevice;
	}

	std::shared_ptr<IVertexBuffer> Graphics::CreateVertexBuffer(VertexBuffer cpuBuffer, std::string tag)
	{
		return std::make_shared<DX11VertexBuffer>(*this, std::move(cpuBuffer), tag);
	}

	std::shared_ptr<IPolyVBuffer> Graphics::CreatePolyVertexBuffer(std::vector<std::variant<std::pair<std::string, VertexBuffer>, std::shared_ptr<IVertexBuffer>>> CpuVBs, std::string tag)
	{
		return std::make_shared<DX11PolyVBuffer>(*this, std::move(CpuVBs), tag);
	}

	std::shared_ptr<IIndexBuffer> Graphics::CreateIndexBuffer(std::shared_ptr<std::vector<int>> indices, std::string tag)
	{
		return std::make_shared<DX11IndexBuffer>(*this, indices, tag);
	}

	std::shared_ptr<IVertexShader> Graphics::CreateVertexShader(std::string path)
	{
		return std::make_shared<DX11VertexShader>(*this, path);
	}

	std::shared_ptr<IPixelShader> Graphics::CreatePixelShader(std::string path)
	{
		return std::make_shared<DX11PixelShader>(*this, path);
	}

	std::shared_ptr<IInputLayout> Graphics::CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs)
	{
		return std::make_shared<DX11InputLayout>(*this, vb, vs);
	}

	std::shared_ptr<IInputLayout> Graphics::CreateInputLayout(IPolyVBuffer& pvb, IVertexShader& vs)
	{
		return std::make_shared<DX11InputLayout>(*this, pvb, vs);
	}

	std::shared_ptr<IPrimitiveTopology> Graphics::CreatePrimitiveTopology()
	{
		return std::make_shared<DX11PrimitiveTopology>(*this);
	}

	std::shared_ptr<IConstantBuffer> Graphics::CreateConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag)
	{
		return std::make_shared<DX11ConstantBuffer>(*this, std::move(layout) , slot , tag);
	}
}