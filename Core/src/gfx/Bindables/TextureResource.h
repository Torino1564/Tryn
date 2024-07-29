#pragma once
#include "Bindable.h"
#include <filesystem>
#include <Core/src/gfx/Texture.h>

namespace tryn::gfx
{
	class ITexture : public IBindable
	{
	public:
		static std::string GenerateID(const IGraphics& gfx, const std::filesystem::path& path, uint8_t slot);
		static std::shared_ptr<ITexture> Resolve(const IGraphics& gfx, const std::filesystem::path& path, uint8_t slot);
		bool HasAlpha() const;
		const Texture& GetTextureResource() const;

	protected:
		std::shared_ptr<Texture> pTextureResource;
		std::string path;
		uint8_t slot = 0;
		bool hasAlpha = false;
	};
}