#pragma once
#include "Bindable.h"
#include <filesystem>
#include <Core/src/gfx/Texture.h>

struct aiTexture;

namespace tryn::gfx
{
	class ITexture : public IBindable
	{
	public:
		static std::string GenerateID(const IGraphics& gfx, const std::filesystem::path& path, uint8_t slot);
		static std::string GenerateID(const IGraphics& gfx, const aiTexture& tex, uint8_t slot);
		static std::string GenerateID(const IGraphics& gfx, const std::shared_ptr<Texture>& pTexture, uint8_t slot);
		static std::shared_ptr<ITexture> Resolve(const IGraphics& gfx, const std::filesystem::path& path, uint8_t slot);
		static std::shared_ptr<ITexture> Resolve(const IGraphics& gfx, const aiTexture& texture, uint8_t slot);
		static std::shared_ptr<ITexture> Resolve(const IGraphics& gfx, std::shared_ptr<Texture> pTexture, uint8_t slot = 0);
		bool HasAlpha() const;
		const Texture& GetTextureResource() const;
		const std::string& GetPath() const;

	protected:
		std::shared_ptr<Texture> pTextureResource;
		std::string path;
		uint8_t slot = 0;
		bool hasAlpha = false;
	};
}