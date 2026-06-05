#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/ConstantBuffer.h>

#include <string_view>

namespace tryn::gfx
{
	class TechniqueProbe;
	class IGraphics;
	class CPUBuffer;

	class IConstantBufferResource : public IBindable
	{
	public:
		enum class Type
		{
			Vertex,
			Pixel
		};

	public:
		static std::string GenerateID(const IGraphics& gfx, const ConstantBufferLayout& cbl, Type type, int slot, const std::string& tag = "?");
		static std::shared_ptr<IConstantBufferResource> Resolve(const IGraphics& gfx, const ConstantBufferLayout& cbl, Type type, int slot, const std::string& tag = "?");
		void Accept_(TechniqueProbe& probe);
		ElementView operator[](const std::string& id);
		virtual void Resize(const std::size_t newSize) = 0;
		std::string_view GetPath() const;
		std::string_view GetTag() const;
		ConstantBuffer& GetConstantBuffer();
		const ConstantBuffer& GetConstantBuffer() const;

	protected:
		Type bindType;
		std::shared_ptr<CPUBuffer> pCPUBuffer;
		std::string path;
		std::string tag;
		uint16_t slot = 0;
		size_t gpuSize = 0;
	};
}