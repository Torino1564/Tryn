#include "Dx11Graphics.h"
#include "GraphicsError.h"
#include <d3dcompiler.h>
#include <Core/src/gfx/dx11/Bindables/DX11InputLayout.h>
#include <Core/src/gfx/dx11/Bindables/DX11VertexShader.h>
#include <Core/src/gfx/dx11/Bindables/DX11PixelShader.h>
#include <Core/src/gfx/dx11/Bindables/DX11PrimitiveTopology.h>
#include <Core/src/gfx/dx11/Bindables/DX11PolyVBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11TransformCBuf.h>
#include <Core/src/gfx/dx11/Bindables/DX11Texture.h>
#include <Core/src/gfx/dx11/Bindables/DX11Rasterizer.h>
#include <Core/src/gfx/dx11/Bindables/DX11Sampler.h>
#include <Core/src/gfx/dx11/Dx11RenderWorker.h>
#include <Core/src/gfx/dx11/Bindables/DX11Buffer.h>
#include "imgui_impl_dx11.h"
#include <Core/src/win/Window.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"D3DCompiler.lib")

namespace tryn::gfx::dx11
{
	Graphics::Graphics(HWND hWnd, int width, int height)
	{
		InitThread();

		auto future = Dispatch_([=, this] {
			
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
			auto tempContext = new DX11Context;
			pThunkContext->QueryInterface(__uuidof(ID3D11DeviceContext), (void**)(tempContext->GetCOMPtr().GetAddressOf())) >> chk;

			//backbuffer
			Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
			pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer) >> chk;
			pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pTarget.ReleaseAndGetAddressOf());

			dimensions.height = height;
			dimensions.width = width;

			//Z Buffer
			D3D11_DEPTH_STENCIL_DESC dsd = {};
			dsd.DepthEnable = TRUE;
			dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsd.DepthFunc = D3D11_COMPARISON_LESS;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
			GetDevice().CreateDepthStencilState(&dsd, &pDSState) >> chk;
			tempContext->GetContext().OMSetDepthStencilState(pDSState.Get(), 1u);

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
			GetDevice().CreateDepthStencilView(pDepthStencil.Get(), &dsvd, &pDSV) >> chk;

			tempContext->GetContext().OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

			//viewport
			viewport.Width = static_cast<float>(width);
			viewport.Height = static_cast<float>(height);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
			tempContext->GetContext().RSSetViewports(1u, &viewport);

			pContext = std::unique_ptr<IContext>(tempContext);
			pSwap->SetFullscreenState((BOOL)false, nullptr) >> chk;

			ImGui_ImplDX11_Init(pDevice.Get(), &GetContext());

			});

		startSignal_.release();
		future.get();
	}

	Graphics::~Graphics()
	{
		Dispatch_([this] {
			ImGui_ImplDX11_Shutdown();
			closing_ = true;
			});
	}

	void Graphics::BeginFrame()
	{
		auto future = Dispatch_([this]
			{
				ImGui_ImplDX11_NewFrame();
				ClearBuffer(0.0f,0.0f,0.2f);
				renderGraph->Reset();
			});
		future.get();
	}

	void Graphics::EndFrame()
	{
		auto future = Dispatch_([=] {
			ExecuteFrame();
			ImGui::EndFrame();
			ImGui::Render();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			auto dd = ImGui::GetDrawData();
			ImGui_ImplDX11_RenderDrawData(dd);
			pSwap->Present(0u, 0u) >> chk;
			});

		future.get();
	}

	void Graphics::ClearBuffer(float r, float g, float b)
	{
		const float color[]{ r, g, b, 1.0f };
		GetContext().ClearRenderTargetView(pTarget.Get(), color);
		GetContext().ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}
	void Graphics::DrawIndexed(int count)
	{
		GetContext().DrawIndexed(count, 0u, 0u);
	}
	constexpr GraphicAPI Graphics::GetType() const
	{
		return GraphicAPI::DX11;
	}
	ID3D11DeviceContext& Graphics::GetContext()
	{
		auto& dx11context = static_cast<DX11Context&>(*pContext);
		return dx11context.GetContext();
	}
	ID3D11Device& Graphics::GetDevice()
	{
		return *pDevice.Get();
	}

	void Graphics::DrawInstancedIndexed(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation, int startInstanceLocation)
	{
		GetContext().DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
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
		auto future = Dispatch_([&] {
			return std::make_shared<DX11VertexBuffer>(*this, pCpuBuffer, tag);

		});
		return future.get();
	}

	//std::shared_ptr<IPolyVBuffer> Graphics::CreatePolyVertexBuffer(std::vector<std::variant<std::pair<std::string, std::shared_ptr<tryn::gfx::VertexBuffer>>, std::shared_ptr<tryn::gfx::IVertexBuffer>, std::shared_ptr<tryn::gfx::IPolyVBuffer>>>& pCpuVBs, std::string tag)
	//{
	//	auto future = Dispatch_([&]{
	//		return std::make_shared<DX11PolyVBuffer>(*this, pCpuVBs, tag);
	//	});
	//	return future.get();
	//}

	std::shared_ptr<IIndexBuffer> Graphics::CreateIndexBuffer(std::shared_ptr<IndexBuffer> indices, std::string tag)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11IndexBuffer>(*this, indices, tag);
			});
		return future.get();
	}

	std::shared_ptr<IVertexShader> Graphics::CreateVertexShader(std::string path)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11VertexShader>(*this, path);
			});
		return future.get();
	}

	std::shared_ptr<IPixelShader> Graphics::CreatePixelShader(std::string path)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11PixelShader>(*this, path);
			});
		return future.get();
	}

	std::shared_ptr<IInputLayout> Graphics::CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11InputLayout>(*this, vb, vs);
			});
		return future.get();
	}

	/*std::shared_ptr<IInputLayout> Graphics::CreateInputLayout(IPolyVBuffer& pvb, IVertexShader& vs)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11InputLayout>(*this, pvb, vs);
			});
		return future.get();
	}*/

	std::shared_ptr<IInputLayout> Graphics::CreateInputLayout(VertexLayout& vLayout, IVertexShader& vs)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11InputLayout>(*this, vLayout, vs);
			});
		return future.get();
	}

	std::shared_ptr<IPrimitiveTopology> Graphics::CreatePrimitiveTopology()
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11PrimitiveTopology>(*this);
			});
		return future.get();
	}

	std::shared_ptr<IVtxConstantBuffer> Graphics::CreateVtxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11VtxConstantBuffer>(*this, std::move(layout), slot, tag);
			});
		return future.get();
	}

	std::shared_ptr<IVtxConstantBufferNCach> Graphics::CreateNonCachVtxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11VtxConstantBufferNCach>(*this, std::move(layout), slot, tag);
			});
		return future.get();
	}

	std::shared_ptr<IPxConstantBuffer> Graphics::CreatePxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11PxConstantBuffer>(*this, std::move(layout), slot, tag);
			});
		return future.get();
	}

	std::shared_ptr<IPxConstantBufferNCach> Graphics::CreateNonCachPxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11PxConstantBufferNCach>(*this, std::move(layout), slot, tag);
			});
		return future.get();
	}

	std::unique_ptr<IInstanceBuffer> Graphics::CreateInstanceBuffer(ConstantBufferLayout&& cbl, int slot)
	{
		cbl.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Bool, "EnabledInstance"));
		auto future = Dispatch_([&] {
			return std::make_unique<DX11InstanceBuffer>(*this, std::move(cbl), slot);
			});
		return future.get();
	}

	std::unique_ptr<ITransformCBuf> Graphics::CreateTransformCBuf()
	{
		auto future = Dispatch_([&] {
			return std::make_unique<DX11TransformCBuf>(*this);
			});
		return future.get();
	}

	std::unique_ptr<RenderWorker> Graphics::CreateRenderWorker(ccr::Master* pMaster)
	{
		return std::make_unique<DX11RenderWorker>(pMaster, *this);
	}

	std::shared_ptr<ITexture> Graphics::CreateTexture(const std::filesystem::path path, const int slot)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11Texture>(*this, path.string(), slot);
			});
		return future.get();
	}

	std::shared_ptr<IRasterizer> Graphics::CreateRasterizer(const bool twoSided)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11Rasterizer>(*this, twoSided);
			});
		return future.get();
	}

	std::shared_ptr<ISampler> Graphics::CreateSampler(SamplerType type, bool reflect, int slot)
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11Sampler>(*this, type, reflect, slot);
			});
		return future.get();
	}
}