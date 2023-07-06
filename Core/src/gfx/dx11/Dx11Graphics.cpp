#include "Dx11Graphics.h"
#include "GraphicsError.h"
#include <iostream>
#include <d3dcompiler.h>
#include <fstream>
#include <Core/src/gfx/Vertex.h>

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
		VertexBuffer vertices(VertexLayout(VertexLayout::Position3D, VertexLayout::Char4Color), 8);
		vertices[0](glm::vec3{-1.0f, -1.0f, -1.0f	}, BGRAColor{ 255,0,0		});
		vertices[1](glm::vec3{1.0f, -1.0f, -1.0f	}, BGRAColor{ 0,255,0		});
		vertices[2](glm::vec3{-1.0f, 1.0f, -1.0f	}, BGRAColor{ 0,0,255		});
		vertices[3](glm::vec3{1.0f, 1.0f, -1.0f		}, BGRAColor{ 255,255,0		});
		vertices[4](glm::vec3{-1.0f, -1.0f, 1.0f	}, BGRAColor{ 0,255,255		});
		vertices[5](glm::vec3{1.0f, -1.0f, 1.0f		}, BGRAColor{ 255,0,255		});
		vertices[6](glm::vec3{-1.0f, 1.0f, 1.0f		}, BGRAColor{ 255,255,255	});
		vertices[7](glm::vec3{1.0f, 1.0f, 1.0f		}, BGRAColor{ 0,255,0		});

		namespace wrl = Microsoft::WRL;

		wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.StructureByteStride = (UINT)vertices.Stride();
		bd.ByteWidth = (UINT)vertices.BufferSize();

		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = vertices.Data();

		pDevice->CreateBuffer(&bd, &srd, &pVertexBuffer) >> chk;
		const UINT stride = (UINT)vertices.Stride();
		const UINT offset = 0u;

		pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		int indeces[] =
		{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};

		wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.StructureByteStride = sizeof(int);
		ibd.ByteWidth = sizeof(indeces);
		D3D11_SUBRESOURCE_DATA isrd = {};
		isrd.pSysMem = indeces;

		pDevice->CreateBuffer(&ibd, &isrd, &pIndexBuffer) >> chk;
		pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);

		// Pixel shader

		wrl::ComPtr<ID3D11PixelShader> pPixelShader;
		wrl::ComPtr<ID3DBlob> pBlob;


		D3DReadFileToBlob(L"PixelShader.cso", &pBlob) >> chk;
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader) >> chk;
		pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);


		// Vertex shader

		wrl::ComPtr<ID3D11VertexShader> pVertexShader;
		D3DReadFileToBlob(L"VertexShader.cso", &pBlob) >> chk;

		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader) >> chk;
		pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

		// Cbuffer
		static float angle = 0;
		struct ConstantBuffer
		{
			glm::mat4 transformation;
		};

		glm::mat4 viewProjection2;
		{
			const auto eyePos = glm::vec3(0, 0, -6);
			const auto focusPoint = glm::vec3(0, 0, 0);
			const auto upDirection = glm::vec3(0, 1, 0);
			const auto view = glm::lookAtLH(eyePos, focusPoint, upDirection);
			const auto projection = glm::perspectiveFovLH(glm::radians(90.0f), (float)dimensions.width, (float)dimensions.height, 0.1f, 100.0f);
			viewProjection2 = projection * view;
		}

		ConstantBuffer cb = {
			{
			glm::transpose( viewProjection2 *
							glm::rotate(glm::mat4(1.0f),0.6f * angle ,glm::vec3(0,0,1.0f) ) *
							glm::rotate(glm::mat4(1.0f), angle ,glm::vec3(1.0f,0,0) ) *
							glm::rotate(glm::mat4(0.5f),2.5f * angle ,glm::vec3(0,1.0f,0)) )
			}
		};

		angle += 0.01;

		wrl::ComPtr<ID3D11Buffer> pCBuffer;
		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.StructureByteStride = 0u;
		cbd.ByteWidth = sizeof(cb);
		D3D11_SUBRESOURCE_DATA csrd = {};
		csrd.pSysMem = &cb;

		pDevice->CreateBuffer(&cbd, &csrd, &pCBuffer) >> chk;

		pContext->VSSetConstantBuffers(0u, 1u, pCBuffer.GetAddressOf());

		// input layout
		wrl::ComPtr<ID3D11InputLayout> pLayout;

		auto layoutBuf = GetLayoutFromVB(vertices);
		auto layout = reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(layoutBuf.data());

		pDevice->CreateInputLayout(layout, (UINT)vertices.GetLayout().GetElementCount(), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pLayout) >> chk;
		pContext->IASetInputLayout(pLayout.Get());

		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);
		pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pContext->DrawIndexed( (UINT)std::size( indeces ) , 0u ,0u);
	}
	void Graphics::DrawIndexed(int count)
	{
		pContext->DrawIndexed(count, 0u, 0u);
	}
	Type Graphics::GetType()
	{
		return Type::DX11;
	}

	DXGI_FORMAT MapDXGIFormat(VertexLayout::Format format)
	{
		switch (format)
		{
		case VertexLayout::Format::Vec2F:
			return DXGI_FORMAT_R32G32_FLOAT;
			break;
		case VertexLayout::Format::Vec3F:
			return DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case VertexLayout::Format::Vec4F:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case VertexLayout::Format::Vec4C_UNorm:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	std::vector<char> Graphics::GetLayoutFromVB(VertexBuffer& vb) const
	{
		const auto& vLayout = vb.GetLayout();
		const auto descSize = vLayout.GetElementCount();
		const auto charVectorSize = descSize * sizeof(D3D11_INPUT_ELEMENT_DESC);

		std::vector<char> layout;
		layout.resize(charVectorSize);

		std::vector<D3D11_INPUT_ELEMENT_DESC> layoutt;
		layout.resize(descSize);
		for (int i = 0; i < descSize; i++)
		{
			auto fakePtr = reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(layout.data() + sizeof(D3D11_INPUT_ELEMENT_DESC) * i);
			fakePtr->SemanticName = vLayout.Elements[i].first.GetName();
			fakePtr->SemanticIndex = vLayout.Elements[i].second;
			fakePtr->Format = MapDXGIFormat(vLayout.Elements[i].first.GetFormat());
			fakePtr->InputSlot = 0u;
			fakePtr->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			fakePtr->AlignedByteOffset = (UINT)vLayout.Elements[i].first.GetOffset();
			fakePtr->InstanceDataStepRate = 0u;
		}

		return layout;
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