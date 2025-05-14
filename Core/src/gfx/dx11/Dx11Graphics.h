#pragma once
#include <Core/src/gfx/IGraphics.h>
#include "DX11RTVDSVFwd.h"
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
		std::shared_ptr<IGenericRenderTargetView> GetRenderTargetView() const override;
		std::shared_ptr<IGenericDepthStencil> GetDepthStencilView() const override;
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
		
		static std::vector<D3D11_INPUT_ELEMENT_DESC> GetSlottedLayout(const VertexLayout& vLayout, int slot);

		// Resource Creation
		std::shared_ptr<IVertexBuffer>						CreateVertexBuffer(std::shared_ptr<VertexBuffer>, std::string) const override;
		std::shared_ptr<ISOAVertexBuffer>					CreateSOAVertexBuffer(const std::shared_ptr<IVertexShader>& pVS) const override;
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
		std::unique_ptr<IInstanceBuffer>					CreateInstanceBuffer(ConstantBufferLayout::Node node, std::size_t size, int slot = 2) const override;
		std::shared_ptr<ITexture>							CreateTexture(std::filesystem::path path, int slot = 0) const override;
		std::shared_ptr<ITexture>							CreateTexture(const aiTexture& tex, int slot = 0) const override;
		std::shared_ptr<ITexture>							CreateTexture(std::shared_ptr<Texture> pTexture, int slot = 0) const override;
		std::shared_ptr<IRasterizer>						CreateRasterizer(const bool twoSided = true) const override;
		std::shared_ptr<ISampler>							CreateSampler(SamplerType type, bool reflect, int slot) const override;
		std::shared_ptr<IOutputOnlyRenderTargetView>		CreateOutputOnlyRenderTargetView(const spa::DimensionsI dimensions) const override;
		std::shared_ptr<IShaderResourceRenderTargetView>	CreateShaderResourceRenderTargetView(const spa::DimensionsI, const uint16_t slot) const override;
		std::shared_ptr<IOutputOnlyDepthStencil>			CreateOutputOnlyDepthStencil(const spa::DimensionsI, ComparissonMode mode = ComparissonMode::Less) const override;
		std::shared_ptr<IShaderResourceDepthStencil>		CreateShaderResourceDepthStencil(const spa::DimensionsI, const uint16_t slot, ComparissonMode mode) const override;
		std::unique_ptr<ITransformCBuf>						CreateTransformCBuf() const override;
		std::unique_ptr<RenderWorker>						CreateRenderWorker(ccr::Master*) const override;

	private:

		static constexpr const char* APIString = "DX11";

		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		std::shared_ptr<DX11OutputOnlyRenderTargetView> pTarget;
		std::shared_ptr<DX11OutputOnlyDepthStencil> pDSV;
		D3D11_VIEWPORT viewport = {};
	};
}