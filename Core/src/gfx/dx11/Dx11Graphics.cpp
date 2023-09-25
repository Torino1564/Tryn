#include "Dx11Graphics.h"
#include "GraphicsError.h"
#include <d3dcompiler.h>
#include <Core/src/gfx/dx11/Bindables/DX11VertexBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11InputLayout.h>
#include <Core/src/gfx/dx11/Bindables/DX11VertexShader.h>
#include <core/src/gfx/dx11/Bindables/DX11IndexBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11PixelShader.h>
#include <Core/src/gfx/dx11/Bindables/DX11ConstantBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11PrimitiveTopology.h>
#include <Core/src/gfx/dx11/Bindables/DX11PolyVBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11TransformCBuf.h>
#include <Core/src/gfx/dx11/Bindables/DX11Texture.h>
#include <Core/src/gfx/dx11/Bindables/DX11Rasterizer.h>
#include <Core/src/gfx/dx11/Bindables/DX11Sampler.h>
#include "imgui_impl_dx11.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

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

		ID3D11DeviceContext* pThunkContext = nullptr;

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
			&pThunkContext
		) >> chk;

		pThunkContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&pContext);

		// backbuffer
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
		pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer) >> chk;
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pTarget.ReleaseAndGetAddressOf());

		dimensions.height = height;
		dimensions.width = width;

		// Z Buffer
		D3D11_DEPTH_STENCIL_DESC dsd = {};
		dsd.DepthEnable = TRUE;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsd.DepthFunc = D3D11_COMPARISON_LESS;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
		pDevice->CreateDepthStencilState(&dsd, &pDSState) >> chk;
		pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC td = {};
		td.Height = height;
		td.Width = width;
		td.MipLevels = 1u;
		td.ArraySize = 1u;
		td.Format = DXGI_FORMAT_D32_FLOAT;
		td.SampleDesc.Count = 1u;
		td.SampleDesc.Quality = 0u;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		pDevice->CreateTexture2D(&td, nullptr, &pDepthStencil) >> chk;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
		dsvd.Format = DXGI_FORMAT_UNKNOWN;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0u;
		pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvd, &pDSV) >> chk;

		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

		// viewport
		D3D11_VIEWPORT vp;
		vp.Width = static_cast<float>(width);
		vp.Height = static_cast<float>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		pContext->RSSetViewports(1u, &vp);

		pSwap->SetFullscreenState((BOOL)false, nullptr) >> chk;

		ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
	}

	Graphics::~Graphics()
	{
		ImGui_ImplDX11_Shutdown();
	}

	void Graphics::BeginFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ClearBuffer(0.0f,0.0f,0.2f);
	}

	void Graphics::EndFrame()
	{
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		pSwap->Present(1u, 0u) >> chk;
	}

	void Graphics::ClearBuffer(float r, float g, float b)
	{
		const float color[]{ r, g, b, 1.0f };

		pContext->ClearRenderTargetView(pTarget.Get(), color);
		pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}
	void Graphics::DrawIndexed(int count)
	{
		pContext->DrawIndexed(count, 0u, 0u);
	}
	GraphicAPI Graphics::GetType()
	{
		return GraphicAPI::DX11;
	}
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1>& Graphics::GetContext()
	{
		return pContext;
	}
	Microsoft::WRL::ComPtr<ID3D11Device>& Graphics::GetDevice()
	{
		return pDevice;
	}

	DXGI_FORMAT Graphics::MapDXGIFormat(VertexLayout::Format format)
	{
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
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> Graphics::GetSlottedLayout(const VertexLayout& vLayout, int slot)
	{
		{
			const auto descSize = vLayout.GetElementCount();

			std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
			for (int i = 0; i < descSize; i++)
			{
				D3D11_INPUT_ELEMENT_DESC descriptor = {};
				descriptor.SemanticName = vLayout.Elements[i].first.GetName();
				descriptor.SemanticIndex = vLayout.Elements[i].second;
				descriptor.Format = MapDXGIFormat(vLayout.Elements[i].first.GetFormat());
				descriptor.InputSlot = (UINT)slot;
				descriptor.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				descriptor.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				descriptor.InstanceDataStepRate = 0u;
				layout.push_back(descriptor);
			}

			return layout;
		}
	}

	std::shared_ptr<IVertexBuffer> Graphics::CreateVertexBuffer(std::shared_ptr<VertexBuffer> pCpuBuffer, std::string tag)
	{
		return std::make_shared<DX11VertexBuffer>(*this, pCpuBuffer, tag);
	}

	std::shared_ptr<IPolyVBuffer> Graphics::CreatePolyVertexBuffer(std::vector<std::variant<std::pair<std::string, std::shared_ptr<tryn::gfx::VertexBuffer>>, std::shared_ptr<tryn::gfx::IVertexBuffer>, std::shared_ptr<tryn::gfx::IPolyVBuffer>>>& pCpuVBs, std::string tag)
	{
		return std::make_shared<DX11PolyVBuffer>(*this, pCpuVBs, tag);
	}

	std::shared_ptr<IIndexBuffer> Graphics::CreateIndexBuffer(std::shared_ptr<const std::vector<int>> indices, std::string tag)
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

	std::shared_ptr<IInputLayout> Graphics::CreateInputLayout(VertexLayout& vLayout, IVertexShader& vs)
	{
		return std::make_shared<DX11InputLayout>(*this, vLayout, vs);
	}

	std::shared_ptr<IPrimitiveTopology> Graphics::CreatePrimitiveTopology()
	{
		return std::make_shared<DX11PrimitiveTopology>(*this);
	}

	std::shared_ptr<IVtxConstantBuffer> Graphics::CreateVtxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag)
	{
		return std::make_shared<DX11VtxConstantBuffer>(*this, std::move(layout), slot, tag);
	}

	std::shared_ptr<IPxConstantBuffer> Graphics::CreatePxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag)
	{
		return std::make_shared<DX11PxConstantBuffer>(*this, std::move(layout), slot, tag);
	}

	std::unique_ptr<ITransformCBuf> Graphics::CreateTransformCBuf()
	{
		return std::make_unique<DX11TransformCBuf>(*this);
	}

	std::shared_ptr<ITexture> Graphics::CreateTexture(const std::filesystem::path path, const int slot)
	{
		return std::make_shared<DX11Texture>(*this, path.string(), slot);
	}
	std::shared_ptr<IRasterizer> Graphics::CreateRasterizer(const bool twoSided)
	{
		return std::make_shared<DX11Rasterizer>(*this, twoSided);
	}
	std::shared_ptr<ISampler> Graphics::CreateSampler(SamplerType type, bool reflect, int slot)
	{
		return std::make_shared<DX11Sampler>(*this, type, reflect, slot);
	}
}