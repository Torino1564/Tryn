
#include "Texture.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/utl/String.h>
#include <assimp/texture.h>
#include "IGraphics.h"
#include "win/gltfSDK.h"
#undef max
#undef min
#include <fastgltf/core.hpp>
#include <fastgltf/util.hpp>
#include <fastgltf/types.hpp>
#include <variant>

namespace tryn::gfx
{
	Texture::Texture(const std::filesystem::path& path)
	{
		auto texture = StbImageManager::Load(path.string(), dimensions, numChannels, StbImageManager::RGB_ALPHA());

		trynass_msg(texture != nullptr, utl::ToWide(std::format("The specified file could not be loaded! File: {}", path.string().c_str())));

		if (numChannels == 4)
		{
			hasAlpha = true;
		}

		this->path = path.string();

		auto pDeleterFunc = [](std::byte* bytes)
			{
				STBI_Close::Get().operator()(bytes);
			};

		static const Deleter deleter(pDeleterFunc);

		buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
	}

	Texture::Texture(const aiTexture& tex)

	{
		this->path = tex.mFilename.C_Str();

		// If the texture is compressed (png, jpeg), resolve to STBI image to decode it 
		if (tex.mHeight == 0)
		{
			// texture is compressed
			auto texture = StbImageManager::Load(std::span{(std::byte*)tex.pcData, tex.mWidth}, dimensions, numChannels, StbImageManager::RGB_ALPHA());

			auto pDeleterFunc = [](std::byte* bytes)
			{
				STBI_Close::Get().operator()(bytes);
			};

			if (numChannels == 4)
			{
				hasAlpha = true;
			}

			static const Deleter deleter(pDeleterFunc);

			buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
		}
		else
		{
			// otherwise load the bytes normally
			const auto bufferSize = tex.mHeight * tex.mWidth * sizeof(aiTexel);
			std::byte* buffer_ = new std::byte[tex.mHeight * tex.mWidth * sizeof(aiTexel)];

			dimensions = {.width = (int)tex.mWidth, .height = (int)tex.mHeight};

			std::memcpy(buffer_, tex.pcData, bufferSize);

			this->path = tex.mFilename.C_Str();
			const auto pDeleterFunc = [](std::byte* bytes)
			{
				free(bytes);
			};

			static const Deleter deleter(pDeleterFunc);

			buffer = std::move(std::unique_ptr<std::byte, Deleter>(buffer_, deleter));
		}
	}

	Texture::Texture(const GLTFTextureData& textureData)
	{
		auto texture = StbImageManager::Load(std::span{(std::byte*)std::move(textureData.data.data()), textureData.byteSize}, dimensions, numChannels, StbImageManager::RGB_ALPHA());
		this->path = textureData.name;

		auto pDeleterFunc = [](std::byte* bytes)
		{
			STBI_Close::Get().operator()(bytes);
		};

		static const Deleter deleter(pDeleterFunc);

		buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
	}

	Texture::Texture(fastgltf::Asset& gltf, fastgltf::Image& image)
	{
		int width, height, nrChannels;

		auto pDeleterFunc = [](std::byte* bytes)
			{
				STBI_Close::Get().operator()(bytes);
			};

		static const Deleter deleter(pDeleterFunc);

		//std::visit(
		//	fastgltf::visitor{
		//		[](auto& arg) {},
		//		[&](fastgltf::sources::URI& filePath) {
		//			trynass(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
		//			trynass(filePath.uri.isLocalPath()); // We're only capable of loading local files.

		//			const std::string path(filePath.uri.path().begin(), filePath.uri.path().end()); // Thanks C++.
		//			auto texture = StbImageManager::Load(path, dimensions, numChannels, StbImageManager::RGB_ALPHA());
		//			buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
		//		},
		//		[&](fastgltf::sources::Vector& vector) {
		//			auto texture = StbImageManager::Load(std::span{vector.bytes.data(), vector.bytes.size()}, dimensions, numChannels, StbImageManager::RGB_ALPHA());
		//			buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
		//		},
		//		[&](fastgltf::sources::BufferView& view) {
		//			auto& bufferView = gltf.bufferViews[view.bufferViewIndex];
		//			auto& buffer_ = gltf.buffers[bufferView.bufferIndex];

		//			std::visit(fastgltf::visitor{
		//				[](auto& arg) {},
		//				[&](fastgltf::sources::Vector& vector) {
		//					auto texture = StbImageManager::Load(std::span{vector.bytes.data(), vector.bytes.size() }, dimensions, numChannels, StbImageManager::RGB_ALPHA());
		//					buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
		//				}
		//				}, buffer_.data);
		//		}
		//	},
		//image.data);

		if (std::holds_alternative<fastgltf::sources::URI>(image.data))
		{
			auto& filePath = std::get<fastgltf::sources::URI>(image.data);
			trynass(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
			trynass(filePath.uri.isLocalPath()); // We're only capable of loading local files.

			const std::string path(filePath.uri.path().begin(), filePath.uri.path().end()); // Thanks C++.
			auto texture = StbImageManager::Load(path, dimensions, numChannels, StbImageManager::RGB_ALPHA());
			buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
		}
		else if (std::holds_alternative<fastgltf::sources::URI>(image.data))
		{
			auto& vector = std::get<fastgltf::sources::Vector>(image.data);
			auto texture = StbImageManager::Load(std::span{ vector.bytes.data(), vector.bytes.size() }, dimensions, numChannels, StbImageManager::RGB_ALPHA());
			buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
		}
		else if (std::holds_alternative<fastgltf::sources::BufferView>(image.data))
		{
			auto& view = std::get<fastgltf::sources::BufferView>(image.data);
			auto& bufferView = gltf.bufferViews[view.bufferViewIndex];
			auto& buffer_ = gltf.buffers[bufferView.bufferIndex];
			if (std::holds_alternative<fastgltf::sources::Vector>(buffer_.data))
			{
				auto& vector = std::get<fastgltf::sources::Vector>(buffer_.data);
				auto texture = StbImageManager::Load(std::span{ vector.bytes.data(), vector.bytes.size() }, dimensions, numChannels, StbImageManager::RGB_ALPHA());
				buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
			}
			if (std::holds_alternative<fastgltf::sources::Array>(buffer_.data))
			{
				auto& array = std::get<fastgltf::sources::Array>(buffer_.data);
				auto texture = StbImageManager::Load(std::span{ array.bytes.data(), array.bytes.size() }, dimensions, numChannels, StbImageManager::RGB_ALPHA());
				buffer = std::move(std::unique_ptr<std::byte, Deleter>(texture, deleter));
			}
		}
	}

	Texture::Texture(const spa::DimensionsI dimensions, const TextureFormat format)
		: dimensions(dimensions), format(format)
	{
		const auto pDeleterFunc = [](std::byte* bytes)
			{
				free(bytes);
			};

		static const Deleter deleter(pDeleterFunc);


		const auto stride = IGraphics::MapTextureFormatStride(format);
		const auto byteSize = dimensions.height * dimensions.width * stride;
		auto data = new std::byte[byteSize];

		buffer = std::move(std::unique_ptr<std::byte, Deleter>(data, deleter));
	}

	std::string Texture::GetID() const noexcept
	{
		return GenerateID(path);
	}

	const std::byte* Texture::Data() const noexcept
	{
		return buffer.get();
	}
	int Texture::GetHeight() const noexcept
	{
		return dimensions.height;
	}
	int Texture::GetWidth() const noexcept
	{
		return dimensions.width;
	}
	int Texture::GetNumChannels() const noexcept
	{
		return StbImageManager::RGB_ALPHA();
	}
	int Texture::GetRowPitch() const noexcept
	{
		return dimensions.width * StbImageManager::RGB_ALPHA();
	}
	bool Texture::HasAlpha() const noexcept
	{
		return hasAlpha;
	}
	const std::string& Texture::GetPath() const
	{
		return path;
	}

	TextureFormat Texture::GetFormat() const
	{
		return format;
	}
}
