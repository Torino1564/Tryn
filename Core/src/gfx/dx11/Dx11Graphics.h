#pragma once
#include <Core/src/gfx/IGraphics.h>
#include "Dx11Context.h"
#include <Core/src/gfx/VertexFormat.h>

namespace tryn::gfx
{
	class VertexBuffer;
}

namespace tryn::win
{
	class Window;
}

template <typename Interface, typename Implementation, typename FunctionArgTuple = void>
	requires std::is_convertible_v<std::add_pointer_t<Implementation>, std::add_pointer_t<Interface>>
struct LinkImplementation
{
	using Interface_t = Interface;
	using Implementation_t = Implementation;
	using FunctionArgTuple_t = FunctionArgTuple;
};

namespace tryn::gfx::dx11
{
	class Graphics : public IGraphics
	{
		friend class DX11Context;
	public:
		Graphics(win::WindowHandle hWnd, int width, int height);
		~Graphics() override;
		void BeginFrame() override;
		void EndFrame() override;
		void ClearBuffer(float r = 0, float g = 0, float b = 0) const override;
		void DrawIndexed(int count) const override;
		void DrawIndexedInstanced(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation, int startInstanceLocation) const override;
		constexpr GraphicAPI GetType() const override;
		ID3D11DeviceContext& GetContext() const;
		ID3D11Device& GetDevice() const;
		IDXGISwapChain& GetSwapChain() const;
		constexpr const char* GetAPIString() const override;
		std::shared_ptr<IRenderTargetView> GetRenderTargetView() const override;
		std::shared_ptr<IDepthStencil> GetDepthStencilView() const override;
		void Resize() override;


		static constexpr DXGI_FORMAT MapDXGIFormat(const VertexFormat format)
		{
			switch (format)
			{
			case VertexFormat::Vec2F:
				return DXGI_FORMAT_R32G32_FLOAT;
				break;
			case VertexFormat::Vec3F:
				return DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case VertexFormat::Vec4F:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case VertexFormat::Vec4C_UNorm:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
				break;
			case VertexFormat::Float_Uint:
				return DXGI_FORMAT_R32_UINT;
				break;
			case VertexFormat::Vec4UI16:
				return DXGI_FORMAT_R16G16B16A16_UINT;
				break;
			default:
				return DXGI_FORMAT_UNKNOWN;
				break;
			}
		}
		static constexpr DXGI_FORMAT MapDXGIFormat(TextureFormat format);
		static std::vector<D3D11_INPUT_ELEMENT_DESC> GetSlottedLayout(const VertexLayout& vLayout, int slot);

		// Resource Creation
		std::shared_ptr<IVertexBuffer>						CreateVertexBuffer(const std::shared_ptr<VertexBuffer>& , std::string) const override;
		std::shared_ptr<ISOAVertexBuffer>					CreateSOAVertexBuffer() const override;
		std::shared_ptr<IIndexBuffer>						CreateIndexBuffer(std::shared_ptr<IndexBuffer> indices, std::string) const override;
		std::shared_ptr<IVertexShader>						CreateVertexShader(std::string path) const override;
		std::shared_ptr<IPixelShader>						CreatePixelShader(std::string path) const override;
		std::shared_ptr<IInputLayout>						CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs) const override;
		std::shared_ptr<IInputLayout>						CreateInputLayout(VertexLayout& vLayout, IVertexShader& vs) const override;
		std::shared_ptr<IPrimitiveTopology>					CreatePrimitiveTopology() const override;
		std::shared_ptr<IVtxConstantBuffer>					CreateVtxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") const override;
		std::shared_ptr<IVtxConstantBufferNCach>			CreateNonCachVtxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") const override;
		std::shared_ptr<IPxConstantBuffer>					CreatePxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") const override;
		std::shared_ptr<IPxConstantBufferNCach>				CreateNonCachPxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") const override;
		std::shared_ptr<IInstanceBuffer>					CreateInstanceBuffer(const ConstantBufferLayout::Node& node, int slot, std::size_t size) const override;
		std::shared_ptr<ITexture>							CreateTexture(std::filesystem::path path, int slot = 0) const override;
		std::shared_ptr<ITexture>							CreateTexture(const aiTexture& tex, int slot = 0) const override;
		std::shared_ptr<ITexture>							CreateTexture(std::shared_ptr<Texture> pTexture, int slot = 0) const override;
		std::shared_ptr<IRasterizer>						CreateRasterizer(const bool twoSided = true) const override;
		std::shared_ptr<ISampler>							CreateSampler(SamplerType type, bool reflect, int slot) const override;
		std::shared_ptr<IRenderTargetView>					CreateRenderTargetView(spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, TextureFormat format) const override;
		std::shared_ptr<IDepthStencil>						CreateDepthStencil(spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, ComparissonMode mode) const override;
		std::shared_ptr<ITransformCBuf>						CreateTransformCBuf() const override;
		std::unique_ptr<IRenderWorker>						CreateRenderWorker(ccr::Master*) const override;
		std::shared_ptr<class DX11InputLayout>				CreateInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descriptorBuffer, const class DX11VertexShader& vs) const;

		class Base {};
		class Derived : public Base {};


		using BindableLinking = std::tuple<
			LinkImplementation<Base, Derived>>;

		template <unsigned N>
		friend void AppendBindableImplementation();

	private:

		static constexpr const char* APIString = "DX11";

		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		std::shared_ptr<class DX11RenderTargetView> pTarget;
		std::shared_ptr<class DX11DepthStencil> pDSV;
		D3D11_VIEWPORT viewport = {};
	};

	constexpr DXGI_FORMAT Graphics::MapDXGIFormat(const TextureFormat format)
	{
		using TF = TextureFormat;
		switch (format)
		{
		case TF::R8G8B8A8_UNORM:         return DXGI_FORMAT_R8G8B8A8_UNORM;
		case TF::B8G8R8A8_UNORM:         return DXGI_FORMAT_B8G8R8A8_UNORM;
		case TF::R8G8B8A8_UNORM_SRGB:    return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case TF::B8G8R8A8_UNORM_SRGB:    return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case TF::R32_FLOAT:              return DXGI_FORMAT_R32_FLOAT;
		case TF::R32G32_FLOAT:           return DXGI_FORMAT_R32G32_FLOAT;
		case TF::R32G32B32_FLOAT:        return DXGI_FORMAT_R32G32B32_FLOAT;
		case TF::R32G32B32A32_FLOAT:     return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case TF::R16_FLOAT:              return DXGI_FORMAT_R16_FLOAT;
		case TF::R16G16_FLOAT:           return DXGI_FORMAT_R16G16_FLOAT;
		case TF::R16G16B16A16_FLOAT:     return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case TF::R32_UINT:               return DXGI_FORMAT_R32_UINT;
		case TF::R32G32_UINT:            return DXGI_FORMAT_R32G32_UINT;
		case TF::R32G32B32_UINT:         return DXGI_FORMAT_R32G32B32_UINT;
		case TF::R32G32B32A32_UINT:      return DXGI_FORMAT_R32G32B32A32_UINT;

		case TF::D32_FLOAT:              return DXGI_FORMAT_D32_FLOAT;
		case TF::D24_UNORM_S8_UINT:      return DXGI_FORMAT_D24_UNORM_S8_UINT;

		case TF::BC1_UNORM:              return DXGI_FORMAT_BC1_UNORM;
		case TF::BC3_UNORM:              return DXGI_FORMAT_BC3_UNORM;
		case TF::BC7_UNORM:              return DXGI_FORMAT_BC7_UNORM;

		default:                         return DXGI_FORMAT_UNKNOWN;
		}
	}
}