#include "TrynPCH.h"
#include "TrynWLR.h"
#include "Dx11Graphics.h"
#include "GraphicsError.h"
#include <d3dcompiler.h>
#include <d3d11_1.h>
#include <Core/src/gfx/dx11/Bindables/DX11InputLayout.h>
#include <Core/src/gfx/dx11/Bindables/DX11VertexShader.h>
#include <Core/src/gfx/dx11/Bindables/DX11PixelShader.h>
#include <Core/src/gfx/dx11/Bindables/DX11PrimitiveTopology.h>
#include <Core/src/gfx/dx11/Bindables/DX11SOAVertexBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11TransformCBuf.h>
#include <Core/src/gfx/dx11/Bindables/DX11Texture.h>
#include <Core/src/gfx/dx11/Bindables/DX11Rasterizer.h>
#include <Core/src/gfx/dx11/Bindables/DX11Sampler.h>
#include <Core/src/gfx/dx11/Dx11RenderWorker.h>
#include <Core/src/gfx/dx11/Bindables/DX11Buffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11RenderTargetView.h>
#include <Core/src/gfx/dx11/Bindables/DX11DepthStencil.h>
#include <imgui_impl_dx11.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/win/TrynWin.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"D3DCompiler.lib")

namespace tryn::gfx::dx11
{
	using BindableLinking = std::tuple<
		LinkImplementation<IVertexShader, DX11VertexShader>,
		LinkImplementation<IPixelShader, DX11PixelShader>
	>;

	template <unsigned N = 0>
	void AppendBindableImplementation(IGraphics::BindableVTable& vtable)
	{
		using namespace std::placeholders;
		if constexpr (N < std::tuple_size_v<BindableLinking>)
		{
			using Pair = std::tuple_element_t<N, BindableLinking>;
			using Interface = typename Pair::Interface_t;
			using Implementation = typename Pair::Implementation_t;
			using FunctionType = std::conditional_t<std::is_same_v<typename Pair::FunctionType_t, void>, decltype(&Interface::Resolve), typename Pair::FunctionType_t>;

			struct Functor
			{
				static std::shared_ptr<Implementation> Impl(const IGraphics& gfx, typename utl::MethodArgTupleMinusFirst<FunctionType>::t params)
				{
					auto future = gfx.Dispatch([&]
						{
							static constexpr auto impl = []<typename... Args>(const Graphics& gfx_, Args&&... unpackedArgs) {
								return std::make_shared<Implementation>(gfx_, std::forward<Args>(unpackedArgs)...);
							};

							return std::apply(std::bind(std::move(impl), std::cref(static_cast<const Graphics&>(gfx)), _1), std::move(params));
						});
					return future.get();
				}
			};

			trylog.info(utl::ToWide(ZT_TYPE_OF(typename utl::MethodArgTupleMinusFirst<FunctionType>::t).data()));
			trylog.info(utl::ToWide(ZT_TYPE_OF(decltype(&Functor::Impl)).data()));
			for (auto& entry : vtable[utl::GetTypeIndex<Interface, IGraphics::SupportedBindables>()])
			{
				if (entry.first != nullptr)
					continue;
				entry = std::pair<void*, utl::UUID_t>(static_cast<void*>(&Functor::Impl), ZT_TYPE_UUID(typename utl::MethodArgTupleMinusFirst<FunctionType>::t));
				break;
			}
			return AppendBindableImplementation<N + 1>(vtable);
		}
	}

	Graphics::Graphics(win::WindowHandle hWnd, int width, int height)
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
			swapDesc.OutputWindow = (HWND)hWnd;
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

			pTarget = std::shared_ptr<DX11RenderTargetView>{ new DX11RenderTargetView(*this, pBackBuffer.Get(), false, {}) };

			dimensions.height = height;
			dimensions.width = width;

			//viewport
			viewport.Width = static_cast<float>(width);
			viewport.Height = static_cast<float>(height);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;

			tempContext->GetContext().RSSetViewports(1u, &viewport);

			pContext = std::unique_ptr<IContext>(tempContext);

			//Z Buffer
			pDSV = std::shared_ptr<DX11DepthStencil>{ new DX11DepthStencil(*this, dimensions, false, {}, ComparissonMode::Less)};

			pSwap->SetFullscreenState((BOOL)false, nullptr) >> chk;

			ImGui_ImplDX11_Init(pDevice.Get(), &GetContext());
			});

		startSignal_.release();
		future.get();
		InitDefaults();

		// init bindable vtable
		AppendBindableImplementation<>(bindableVtable);
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
				//ClearBuffer(0.0f,0.0f,0.2f);
				pRenderGraph->Reset();
			});
		future.get();
	}

	void Graphics::EndFrame()
	{
		auto future = Dispatch_([=] {
			ExecuteFrame();
			ImGui::Render();
			auto dd = ImGui::GetDrawData();
			ImGui_ImplDX11_RenderDrawData(dd);
			pSwap->Present(vsync ? 1u : 0u, 0u) >> chk;

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			});

		future.get();
	}

	void Graphics::ClearBuffer(float r, float g, float b) const
	{
		const float color[]{ r, g, b, 1.0f };
		GetContext().ClearRenderTargetView(pTarget->Get(), color);
		GetContext().ClearDepthStencilView(pDSV->Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}
	void Graphics::DrawIndexed(int count) const
	{
		GetContext().DrawIndexed(count, 0u, 0u);
	}
	constexpr GraphicAPI Graphics::GetType() const
	{
		return GraphicAPI::DX11;
	}
	ID3D11DeviceContext& Graphics::GetContext() const
	{
		auto& dx11context = static_cast<DX11Context&>(*pContext);
		return dx11context.GetContext();
	}
	ID3D11Device& Graphics::GetDevice() const
	{
		return *pDevice.Get();
	}
	IDXGISwapChain& Graphics::GetSwapChain() const
	{
		return *pSwap.Get();
	}
	constexpr const char* Graphics::GetAPIString() const
	{
		return APIString;
	}
	std::shared_ptr<IRenderTargetView> Graphics::GetRenderTargetView() const
	{
		return pTarget;
	}
	std::shared_ptr<IDepthStencil> Graphics::GetDepthStencilView() const
	{
		return pDSV;
	}

	void Graphics::Resize()
	{
		auto future = Dispatch_([&] {
			// Release resources
			pTarget->Release();
			pDSV->Release();

			pContext->ClearState();
			pContext->Flush();

			pSwap->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0) >> chk;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
			pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer) >> chk;
			D3D11_TEXTURE2D_DESC tDesc = {};
			pBackBuffer->GetDesc(&tDesc);
			dimensions = {.width = (int)tDesc.Width, .height = (int)tDesc.Height};

			pTarget->RegenerateResources(pBackBuffer.Get());
			pDSV->RegenerateResource(dimensions);

			//viewport
			viewport.Width = static_cast<float>(dimensions.width);
			viewport.Height = static_cast<float>(dimensions.height);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;

			pContext->UpdateContextDimensions(*this);
		});
		return future.get();
	}

	void Graphics::DrawIndexedInstanced(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation, int startInstanceLocation) const
	{
		pContext->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> Graphics::GetSlottedLayout(const VertexLayout& vLayout, int slot)
	{
		{
			const auto descSize = vLayout.GetElementCount();

			std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
			for (int i = 0; i < descSize; i++)
			{
				D3D11_INPUT_ELEMENT_DESC descriptor = {};
				descriptor.SemanticName = vLayout.Elements[i].GetName();
				descriptor.SemanticIndex = vLayout.Elements[i].Index();
				descriptor.Format = MapDXGIFormat(vLayout.Elements[i].GetFormat());
				descriptor.InputSlot = (UINT)slot;
				descriptor.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				descriptor.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				descriptor.InstanceDataStepRate = 0u;
				layout.push_back(descriptor);
			}

			return layout;
		}
	}

	std::shared_ptr<IVertexBuffer> Graphics::CreateVertexBuffer(const std::shared_ptr<VertexBuffer>& pCpuBuffer, std::string tag) const 
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11VertexBuffer>(*this, pCpuBuffer, tag);
		});
		return future.get();
	}

	std::shared_ptr<ISOAVertexBuffer> Graphics::CreateSOAVertexBuffer() const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11SOAVertexBuffer>(*this);

			});
		return future.get();
	}

	std::shared_ptr<IIndexBuffer> Graphics::CreateIndexBuffer(std::shared_ptr<IndexBuffer> indices, std::string tag) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11IndexBuffer>(*this, indices, tag);
			});
		return future.get();
	}

	std::shared_ptr<IVertexShader> Graphics::CreateVertexShader(std::string path) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11VertexShader>(*this, path);
			});
		return future.get();
	}

	std::shared_ptr<IPixelShader> Graphics::CreatePixelShader(std::string path) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11PixelShader>(*this, path);
			});
		return future.get();
	}

	std::shared_ptr<IInputLayout> Graphics::CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11InputLayout>(*this, vb, vs);
			});
		return future.get();
	}

	std::shared_ptr<IInputLayout> Graphics::CreateInputLayout(VertexLayout& vLayout, IVertexShader& vs) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11InputLayout>(*this, vLayout, vs);
			});
		return future.get();
	}

	std::shared_ptr<IPrimitiveTopology> Graphics::CreatePrimitiveTopology() const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11PrimitiveTopology>(*this);
			});
		return future.get();
	}

	std::shared_ptr<IVtxConstantBuffer> Graphics::CreateVtxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11VtxConstantBuffer>(*this, std::move(layout), slot, tag);
			});
		return future.get();
	}

	std::shared_ptr<IVtxConstantBufferNCach> Graphics::CreateNonCachVtxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11VtxConstantBufferNCach>(*this, std::move(layout), slot, tag);
			});
		return future.get();
	}

	std::shared_ptr<IPxConstantBuffer> Graphics::CreatePxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11PxConstantBuffer>(*this, std::move(layout), slot, tag);
			});
		return future.get();
	}

	std::shared_ptr<IPxConstantBufferNCach> Graphics::CreateNonCachPxConstantBuffer(ConstantBufferLayout&& layout, int slot, std::string tag) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11PxConstantBufferNCach>(*this, std::move(layout), slot, tag);
			});
		return future.get();
	}

	std::shared_ptr<IInstanceBuffer> Graphics::CreateInstanceBuffer(ConstantBufferLayout::Node node, std::size_t size, int slot) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11InstanceBuffer>(*this, node, slot, size);
			});
		return future.get();
	}

	std::shared_ptr<IDepthStencil> Graphics::CreateDepthStencil(spa::DimensionsI dimensions, bool shaderResource,
		std::optional<uint16_t> slot, ComparissonMode mode) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11DepthStencil>(*this, dimensions, shaderResource, slot, mode);
			});
		return future.get();
	}

	std::shared_ptr<ITransformCBuf> Graphics::CreateTransformCBuf() const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11TransformCBuf>(*this);
			});
		return future.get();
	}

	std::unique_ptr<IRenderWorker> Graphics::CreateRenderWorker(ccr::Master* pMaster) const
	{
		return std::make_unique<DX11RenderWorker>(pMaster, *this);
	}

	std::shared_ptr<DX11InputLayout> Graphics::CreateInputLayout(
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& descriptorBuffer, const DX11VertexShader& vs) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11InputLayout>(*this, descriptorBuffer, vs);
			});
		return future.get();
	}

	std::shared_ptr<ITexture> Graphics::CreateTexture(const std::filesystem::path path, const int slot) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11Texture>(*this, path.string(), slot);
			});
		return future.get();
	}

	std::shared_ptr<ITexture> Graphics::CreateTexture(const aiTexture& tex, int slot) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11Texture>(*this, tex, slot);
			});
		return future.get();
	}

	std::shared_ptr<ITexture> Graphics::CreateTexture(std::shared_ptr<Texture> pTexture, int slot) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11Texture>(*this, pTexture, slot);
			});
		return future.get();
	}

	std::shared_ptr<IRasterizer> Graphics::CreateRasterizer(const bool twoSided) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11Rasterizer>(*this, twoSided);
			});
		return future.get();
	}

	std::shared_ptr<ISampler> Graphics::CreateSampler(SamplerType type, bool reflect, int slot) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11Sampler>(*this, type, reflect, slot);
			});
		return future.get();
	}

	std::shared_ptr<IRenderTargetView> Graphics::CreateRenderTargetView(spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, TextureFormat format) const
	{
		auto future = Dispatch_([&] {
			return std::make_shared<DX11RenderTargetView>(*this, dimensions, shaderResource, slot, format);
			});
		return future.get();
	}
}
