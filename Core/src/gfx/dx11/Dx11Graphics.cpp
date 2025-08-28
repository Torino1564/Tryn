
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

using namespace std::placeholders;

namespace tryn::gfx::dx11
{
	// Register Bindable Implementations
	using BindableLinking = std::tuple <
		LinkImplementation<IVertexShader, DX11VertexShader>,
		LinkImplementation<IPixelShader, DX11PixelShader>,
		LinkImplementation<IPrimitiveTopology, DX11PrimitiveTopology>,
		LinkImplementation<ITransformCBuf, DX11TransformCBuf>,
		LinkImplementation<IVertexBuffer, DX11VertexBuffer>,
		LinkImplementation<IDepthStencil, DX11DepthStencil>,
		LinkImplementation<IRasterizer, DX11Rasterizer>,
		LinkImplementation<ISampler, DX11Sampler>,
		LinkImplementation<IRenderTargetView, DX11RenderTargetView>,
		LinkImplementation<ISOAVertexBuffer, DX11SOAVertexBuffer>,
		LinkImplementation<IIndexBuffer, DX11IndexBuffer>,
		LinkImplementation<IVtxConstantBuffer, DX11VtxConstantBuffer>,
		LinkImplementation<IVtxConstantBufferNCach, DX11VtxConstantBufferNCach>,
		LinkImplementation<IPxConstantBuffer, DX11PxConstantBuffer>,
		LinkImplementation<IPxConstantBufferNCach, DX11PxConstantBufferNCach>,
		LinkImplementation<IInstanceBuffer, DX11InstanceBuffer>,
		LinkImplementation<IInputLayout, DX11InputLayout>,
		LinkImplementation<ITexture, DX11Texture>
	>;

	template <typename Interface, typename Implementation, typename FunctionArgTuple, bool HasArgs>
	struct BindableFunctor;

	template <typename Interface, typename Implementation, typename FunctionArgTuple>
	struct BindableFunctor<Interface, Implementation, FunctionArgTuple, true>
	{
		static std::shared_ptr<Interface> Impl(const IGraphics& gfx, FunctionArgTuple params)
		{
			trylog.debug(utl::ToWide(ZT_TYPE_OF(FunctionArgTuple).data()));
			auto future = gfx.Dispatch([&]
				{
					static constexpr auto impl = []<typename... Args>(Args&&... unpackedArgs)
					{
						(trylog.debug(utl::ToWide(ZT_TYPE_OF(decltype(unpackedArgs)).data())), ...);
						return std::make_shared<Implementation>(std::forward<Args>(unpackedArgs)...);
					};
					return std::apply(std::move(impl), std::tuple_cat(std::move(std::tuple(std::cref(static_cast<const Graphics&>(gfx)))), std::move(params)));
				});
			return future.get();
		}
	};

	template <typename Interface, typename Implementation>
	struct BindableFunctor<Interface, Implementation, std::tuple<>, false>
	{
		static std::shared_ptr<Interface> Impl(const IGraphics& gfx, std::tuple<>)
		{
			auto future = gfx.Dispatch([&]
				{
					return std::make_shared<Implementation>(static_cast<const Graphics&>(gfx));
				});
			return future.get();
		}
	};

	template<typename Interface, typename Implementation, typename ParamTuplesTuple, unsigned N = 0>
	static void AppendNonDefaultResolvableImplementation(std::array<std::any, IGraphics::MAX_CONSTRUCTORS_ALLOWED>& vtableEntry)
	{
		trylog.debug(utl::ToWide(ZT_TYPE_OF(ParamTuplesTuple).data()));
		if constexpr (N < std::tuple_size_v<ParamTuplesTuple>)
		{
			using FunctionArgTuple = std::tuple_element_t<N, ParamTuplesTuple>;
			trylog.debug(utl::ToWide(ZT_TYPE_OF(FunctionArgTuple).data()));
			//if constexpr (utl::is_constructible_from_tuple_v<Implementation, FunctionArgTuple>)
			{
				using Functor = BindableFunctor<Interface, Implementation, FunctionArgTuple, (std::tuple_size_v<FunctionArgTuple> != 0)>;

				for (auto& entry : vtableEntry)
				{
					if (entry.has_value())
						continue;

					entry = { &Functor::Impl };
					break;
				}
			}
			return AppendNonDefaultResolvableImplementation<Interface, Implementation, ParamTuplesTuple, N + 1>(vtableEntry);
		}
	}

	template <unsigned N = 0>
	static void AppendBindableImplementation(IGraphics::BindableVTable& vtable)
	{
		if constexpr (N < std::tuple_size_v<BindableLinking>)
		{
			trylog.debug(std::to_wstring(N));
			using Pair = std::tuple_element_t<N, BindableLinking>;
			using Interface = typename Pair::Interface_t;
			using Implementation = typename Pair::Implementation_t;

			using ParamTuplesTuple = typename std::tuple_element_t<utl::GetTypeIndexFromTupleOfRegister<Interface, IGraphics::SupportedBindables>(), IGraphics::SupportedBindables>::ParameterTuplesTuple_t;
			auto& vtableEntry = vtable[utl::GetTypeIndexFromTupleOfRegister<Interface, IGraphics::SupportedBindables>()];
			if constexpr (std::tuple_size_v<ParamTuplesTuple> == 0)
			{
				// Resort to Resolve Method
				using FunctionArgTuple = typename utl::MethodArgTupleMinusFirst<decltype(&Interface::Resolve)>::t;
				using Functor = BindableFunctor<Interface, Implementation, FunctionArgTuple, (std::tuple_size_v<FunctionArgTuple> != 0)>;

				for (auto& entry : vtableEntry)
				{
					if (entry.has_value())
						continue;

					entry = { &Functor::Impl };
					break;
				}
				return AppendBindableImplementation<N + 1>(vtable);
			}
			else
			{
				// Check all other combinations and append
				AppendNonDefaultResolvableImplementation<Interface, Implementation, ParamTuplesTuple>(vtableEntry);
				return AppendBindableImplementation<N + 1>(vtable);
			}
		}
	}

	Graphics::Graphics(win::WindowHandle hWnd, int width, int height)
	{
		InitThread();

		// init bindable vtable
		AppendBindableImplementation<>(bindableVtable);
		static constexpr auto swapFormat = TextureFormat::B8G8R8A8_UNORM;
		auto future = Dispatch_([=, this] {

			DXGI_SWAP_CHAIN_DESC swapDesc = {};
			swapDesc.BufferDesc.Width = width;
			swapDesc.BufferDesc.Height = height;
			swapDesc.BufferDesc.Format = MapDXGIFormat(swapFormat);
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

			//backbuffer
			Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
			pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer) >> chk;

			pContext = std::unique_ptr<IContext>(tempContext);

			pTarget = std::make_shared<DX11RenderTargetView>(*this, pBackBuffer.Get(), 0);

			//Z Buffer
			pDSV = std::shared_ptr<DX11DepthStencil>{ new DX11DepthStencil(*this, dimensions, false, {}, ComparissonMode::Less) };

			pSwap->SetFullscreenState((BOOL)false, nullptr) >> chk;

			ImGui_ImplDX11_Init(pDevice.Get(), &GetContext());
			});

		startSignal_.release();
		future.get();
		InitDefaults();
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
			dimensions = { .width = static_cast<int>(tDesc.Width), .height = static_cast<int>(tDesc.Height) };

			pTarget->RegenerateResources(dimensions);
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

	void Graphics::DrawIndexedInstanced(const int indexCount, const int instanceCount, const int startIndexLocation,
		const int baseVertexLocation, const int startInstanceLocation) const
	{
		pContext->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> Graphics::GetSlottedLayout(const VertexLayout& vLayout, const int slot)
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
				descriptor.InputSlot = static_cast<UINT>(slot);
				descriptor.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				descriptor.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				descriptor.InstanceDataStepRate = 0u;
				layout.push_back(descriptor);
			}

			return layout;
		}
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
}
