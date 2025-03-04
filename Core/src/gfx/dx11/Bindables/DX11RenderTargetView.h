#pragma once
#include <Core/src/gfx/bindables/RenderTargetView.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/utl/EmptyType.h>
#include <Core/src/gfx/dx11/DX11RTVDSVFwd.h>
#include <Core/src/gfx/dx11/Bindables/DX11DepthStencil.h>

namespace tryn::gfx::dx11
{
	template <BufferResourceType Type>
	class DX11RenderTargetView : public IRenderTargetView<Type>
	{
	public:
		DX11RenderTargetView(const Graphics& gfx, const spa::DimensionsI dimensions)
			requires (Type == BufferResourceType::OutputOnly);
		DX11RenderTargetView(const Graphics& gfx, const spa::DimensionsI dimensions, uint16_t slot)
			requires (Type == BufferResourceType::ShaderResource);
		DX11RenderTargetView(const Graphics& gfx, ID3D11Texture2D* pTexture)
			requires (Type == BufferResourceType::OutputOnly);
		void BindAsRTV(IGenericDepthStencil* pDSV) override;
		void Bind() override;
		void Bind(const class IContext& ctx) override;
		ID3D11RenderTargetView* Get() const;
		ID3D11RenderTargetView* const* GetAddressOf() const;
		ID3D11RenderTargetView** GetAddressOf();
		void Clear() const override;
		void Release();

		void RegenerateResources(const spa::DimensionsI dimensions);
		void RegenerateResources(ID3D11Texture2D* pTextureIn);

	private:
		void RTVCreation(const Graphics& gfx, const spa::DimensionsI dimensions);
		void RTVCreation(ID3D11Texture2D* pTextureIn);
		void SRVCreation(const Graphics& gfx, uint16_t slot)
			requires (Type == BufferResourceType::ShaderResource);


		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;
		[[no_unique_address]] std::conditional_t<Type == BufferResourceType::ShaderResource, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, utl::empty_t> pSRV;
	};
}
