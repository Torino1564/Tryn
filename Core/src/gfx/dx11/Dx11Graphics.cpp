#include "Dx11Graphics.h"
#include "GraphicsError.h"
#include <iostream>
#include <d3dcompiler.h>
#include <fstream>


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
		struct Vertex {
			float x;
			float y;
		};

		Vertex vertices[] =
		{
			{ 0.0f , 0.5f },
			{ 0.5f , -0.5f },
			{ -0.5f , -0.5f }
		};

		namespace wrl = Microsoft::WRL;

		wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.StructureByteStride = sizeof(Vertex);
		bd.ByteWidth = sizeof(vertices);

		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = vertices;

		pDevice->CreateBuffer(&bd, &srd, &pVertexBuffer) >> chk;
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;

		pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		wrl::ComPtr<ID3D11PixelShader> pPixelShader;
		wrl::ComPtr<ID3DBlob> pBlob;

		// Pixel shader

		D3DReadFileToBlob(L"PixelShader.cso", &pBlob) >> chk;
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader) >> chk;
		pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);


		// Vertex shader

		wrl::ComPtr<ID3D11VertexShader> pVertexShader;
		D3DReadFileToBlob(L"VertexShader.cso", &pBlob) >> chk;

		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader) >> chk;
		pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

		// input layout
		wrl::ComPtr<ID3D11InputLayout> pLayout;
		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};

		pDevice->CreateInputLayout(layout, (UINT)std::size(layout), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pLayout) >> chk;
		pContext->IASetInputLayout(pLayout.Get());

		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);
		pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// COnfigure viewport

		D3D11_VIEWPORT vp;
		vp.Width = 800;
		vp.Height = 600;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		pContext->RSSetViewports(1u, &vp);

		pContext->Draw( (UINT)std::size( vertices ) , 0u );
	}
}