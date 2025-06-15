#pragma once
#include "Bindable.h"
#include <Core/src/spa/Dimensions.h>
#include <memory>
#include <Core/src/gfx/TextureFormat.h>

#include "DepthStencil.h"

namespace tryn::gfx
{
	class ITexture;
	class IDepthStencil;
	class IRenderTargetView : public IBindable
	{
	public:
		static std::string GenerateID(const IGraphics& gfx, const std::shared_ptr<ITexture>& pTexture, uint16_t slot);
		static std::shared_ptr<IRenderTargetView> Resolve(const IGraphics& gfx, const std::shared_ptr<ITexture>& pTexture, uint16_t slot);

		virtual void Clear() const = 0;
		void SetDepthStencil(IDepthStencil& dsv);
		ITexture& GetTexture();
		const ITexture& GetTexture() const;

	protected:
		std::shared_ptr<ITexture> pTexture;
		IDepthStencil* pDSV = nullptr;
		uint16_t slot = 0;
	};
}
