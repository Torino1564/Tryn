#pragma once
#include <Core/src/gfx/bindables/DepthStencil.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/utl/EmptyType.h>
#include <Core/src/gfx/dx11/DX11RTVDSVFwd.h>

namespace tryn::gfx::dx11
{
	template <BufferResourceType Type>
	class DX11DepthStencil : public IDepthStencil<Type>
	{
	public:
		ID3D11DepthStencilView* Get() const;
		ID3D11DepthStencilView** GetAddressOf();
		DX11DepthStencil(const Graphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::OutputOnly);
		DX11DepthStencil(const Graphics& gfx, const spa::DimensionsI dimensions, const uint16_t slot, ComparissonMode mode = ComparissonMode::Less)
			requires (Type == BufferResourceType::ShaderResource);
		void Bind() override;
		void Bind(const class IContext& ctx) override;
		void Clear() const override;
		void Release();

		void RegenerateResource(const spa::DimensionsI dimensions);
	private:
		void DSVCreation(const Graphics& gfx, const spa::DimensionsI dimensions, ComparissonMode mode, bool isShaderResource);
		void SRVCreation(const Graphics& gfx, const uint16_t slot)
			requires (Type == BufferResourceType::ShaderResource);

		ComparissonMode mode = ComparissonMode::Less;
		const Graphics& gfx;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
		[[no_unique_address]] std::conditional_t<Type == BufferResourceType::ShaderResource, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, utl::empty_t> pSRV;

	};
}
